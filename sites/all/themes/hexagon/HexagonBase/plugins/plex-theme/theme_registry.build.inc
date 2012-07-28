<?php

/**
 * @file
 * Alters the theme registry.
 *
 * This file should not be included in every page request.
 */

/**
 * implements hook_theme_registry_alter()
 */
function plex_theme_theme_registry_alter(&$registry) {
  // Prep.
  $active_theme   = hex()->theme();
  $active_plugins = hex()->listType('plugin');

  // Settings.
  $suggestions_handles_base = hex_settings('plex_theme.core_fix:suggestions_handles_base', HEX_FIRST_PARENT);
  $base_handles_suggestions = hex_settings('plex_theme.core_fix:base_handles_suggestions', HEX_FIRST_PARENT);

  // Register hook patterns based on variable processors and .vars.inc files.
  if (hex_settings('plex_theme.implicit_hook_registration', HEX_FIRST_PARENT)) {
    plex_theme_implicit_hook_registration($registry);
  }

  // Get all include files.
  // The type of files gathered will depend on specific settings:
  // - 'plex_theme.load_on_demand:theme_inc' $includes->theme_inc == *.theme.inc
  // - 'plex_theme.load_on_demand:func_inc'  $includes->func_inc  == *.func.inc
  // - 'plex_theme.load_on_demand:vars_inc'  $includes->vars_inc  == *.vars.inc
  $includes = plex_theme_get_includes_by_type($registry);

  foreach ($registry as $theme_hook => &$hook_data) {
    // Get contextual data for the hook.
    $hook_context = plex_theme_hook_context($registry, $theme_hook);

    // Initialize. Make sure these elements are available.
    // Empty elements will be removed later.
    $hook_includes = &$hook_data['includes'];
    $vars_includes = &$hook_data['vars includes'];
    $preprocesses  = &$hook_data['preprocess functions'];
    $processes     = &$hook_data['process functions'];

    foreach (array('includes', 'vars includes', 'preprocess functions', 'process functions') as $key) {
      if (!isset($hook_data[$key])) {
        $hook_data[$key] = array();
      }
    }

    // This is used to invoke variable processors when hook suggestions are
    // involved. It works on two levels:
    //
    // 1. When theme('hook__suggestion') is invoked, the variable processors
    //    for that hook will not be complete. What this does is add specialzed
    //    processors (plex_theme_variable_preprocess_base &
    //    plex_theme_variable_process_base) to handle the invocation of the
    //    base functions. It does this by calling the processors for the base
    //    hook effectively providing a complete set of variable processors.
    // 2. When the suggestion is set from a variable processor, it prevents
    //    the hook suggestion from having its own variable processors from
    //    being invoked since it's in the same variable processing scope. What
    //    this does is add a specialized processor
    //    (plex_theme_variable_preprocess_suggestion &
    //    plex_theme_variable_process_suggestion) which handles the
    //    invocation of the remaining processors for the suggestion.

    // pre/process functions used by the plex_theme plug-in should be added
    // to $ctl_pre/processes.
    $ctl_preprocesses = &$preprocesses;
    $ctl_processes    = &$processes;

    // Current hook is a base hook and has patterns/suggestions based on it.
    if ($base_handles_suggestions && $hook_context->is_base && $hook_context->has_suggestions) {
      $ctl_preprocesses[]  = 'plex_theme_variable_preprocess_suggestion';
      $ctl_processes[]     = 'plex_theme_variable_process_suggestion';
    }
    // Current hook is based on a pattern.
    elseif ($suggestions_handles_base && !$hook_context->is_base) {
      // Connect existing 'pre/process functions' to 'suggested pre/process
      // functions'.
      if ($preprocesses) {
        $hook_data['suggested preprocess functions'] = $hook_data['preprocess functions'];
        unset($hook_data['preprocess functions']);
        $preprocesses     = &$hook_data['suggested preprocess functions'];
        $ctl_preprocesses = &$hook_data['preprocess functions'];
      }
      if ($processes) {
        $hook_data['suggested process functions'] = $hook_data['process functions'];
        unset($hook_data['process functions']);
        $processes     = &$hook_data['suggested process functions'];
        $ctl_processes = &$hook_data['process functions'];
      }
      // This references the original 'pre/process functions' key.
      $ctl_preprocesses = array('plex_theme_variable_preprocess_base');
      $ctl_processes    = array('plex_theme_variable_process_base');
    }

    // Adds files which will be included automatically from a theme() call.

    // Include:
    // - [ MODULE | THEME | PLUGIN ].theme.php
    // Active when 'plex_theme.enable_theme_inc' setting is true. (defaults to true)
    if (isset($includes->theme_inc[$theme_hook])) {
      $hook_includes = array_merge($hook_includes, $includes->theme_inc[$theme_hook]);
    }

    // Include:
    // - [ HOOK ].func.php
    // Active when 'plex_theme.enable_func_inc' setting is true. (defaults to true)
    if ($hook_context->implement_type === 'func' && isset($includes->func_inc[$theme_hook])) {
      // Use last element which will be in the child-most theme.
      $hook_includes[] = end($includes->func_inc[$theme_hook]);
    }

    // The following files will host variables specific to the theming hook
    // whether it is implemented as a function or template. Some are more
    // focused than others.
    //
    // Process:
    // - [ HOOK ].vars.php
    // - [ HOOK ].[ ACTIVE-THEME ].vars.php
    // - [ HOOK ].[ func | tpl ].vars.php
    // - [ HOOK ].[ ACTIVE-THEME ].[ func | tpl ].vars.php
    foreach (array($theme_hook,
                  "$theme_hook.$active_theme",
                  "$theme_hook.{$hook_context->implement_type}",
                  "$theme_hook.$active_theme.{$hook_context->implement_type}") as $named) {

      // Active when 'plex_theme.enable_vars_inc' setting is true. (defaults to true)
      if (isset($includes->vars_inc[$named])) {
        $vars_includes = array_merge($vars_includes, array_values($includes->vars_inc[$named]));
      }
    }

    // Add the processor responsible for including the .vars.php files.
    if ($vars_includes) {
      // Move to the front of second phase process so it always comes after
      // preprocess functions but before process functions.
      array_unshift($ctl_processes, 'plex_theme_variable_include_process');
    }

    // Add in variable process functions for plugins. Core only knows of
    // modules and themes so plug-ins are added here manually.
    foreach ($active_plugins as $prefix) {

      $var_preproc_func = "{$prefix}_preprocess";
      $var_proc_func    = "{$prefix}_process";

      // PLUGIN_preprocess/PLUGIN_process() pushed up front.
      // Must be a base hook and be implemented as a template.
      if ($hook_context->is_base && isset($hook_data['template'])) {
        if (hex_function_exists($var_preproc_func)) {
          array_unshift($preprocesses, $var_preproc_func);
        }
        if (hex_function_exists($var_proc_func)) {
          array_unshift($processes, $var_proc_func);
        }
      }

      // PLUGIN_preprocess_HOOK/PLUGIN_process_HOOK() come last.
      if (hex_function_exists($var_preproc_func .= "_$theme_hook")) {
        $preprocesses[] = $var_preproc_func;
      }
      if (hex_function_exists($var_proc_func .= "_$theme_hook")) {
        $processes[] = $var_proc_func;
      }
    }

    // Optimize the registry.
    foreach (array('includes', 'vars includes', 'preprocess functions', 'process functions') as $key) {
      if (isset($hook_data[$key]) && empty($hook_data[$key])) {
        unset($hook_data[$key]);
      }
    }

    if ($suggestions_handles_base) {
      // The 'base hook' is poorly handed in core's theme function so replace it.
      if (isset($hook_data['base hook'])) {
        unset($hook_data['base hook']);
      }
      // This is used in plex_theme_variable_preprocess_base and plex_theme_variable_process_base.
      if (!$hook_context->is_base) {
        $hook_data['plex base'] = $hook_context->base_hook;
      }
    }
  }
}

/**
 * Builds extra context about a theming hook for the theme registry.
 */
function plex_theme_hook_context($registry, $theme_hook) {
  $hook_origins = plex_theme_get_hook_origins();

  $host_types = array(
    'module'            => 'module',
    'base_theme_engine' => 'theme',
    'base_theme'        => 'theme',
    'theme_engine'      => 'theme',
    'theme'             => 'theme',
    'base_theme_plugin' => 'plugin',
    'theme_plugin'      => 'plugin',
  );

  $hook_data = $registry[$theme_hook];

  $hook_context = new stdClass();
  $hook_context->host_type      = $host_types[$hook_data['type']];
  $hook_context->implement_type = isset($hook_data['function']) ? 'func' : 'tpl';

  // Find base hook for hook patterns.
  $base_hook = plex_theme_find_parent_hook($hook_origins, $theme_hook);
  $hook_context->is_base = !$base_hook;
  if ($base_hook) {
    $hook_context->base_hook = $base_hook;
  }

  // Identify the origin of the hook.
  $hook_context->origin = $base_hook ? $hook_origins[$base_hook] : $hook_origins[$theme_hook];

  // Check if there's a derivative "suggested" hook for base hooks.
  if ((bool) hex_settings('plex_theme.core_fix:base_handles_suggestions', HEX_FIRST_PARENT) && $hook_context->is_base) {
    $hook_context->has_suggestions = FALSE;
    foreach (preg_grep("/^($theme_hook)__/", array_keys($registry)) as $suggestion) {
      // Ensure the found suggestion is not another base hook. 'links__ctools_dropbutton'
      // from ctools is an example of this. It does not directly extend the 'links' hook
      // so it should not even attempt to see it as a derivative.
      if (!isset($hook_origins[$suggestion])) {
        $hook_context->has_suggestions = TRUE;
        break;
      }
    }
  }

  return $hook_context;
}

/**
 * Registers hook patterns or 'suggestions' that are not explicitly registered
 * through theme functions or .tpl.php files.
 *
 * Registration is based on hook specific variable processors and .vars.php
 * files as long as it is based on an existing 'base hook'. For example, the
 * 'page' base hook can have an derivative 'page__story' hook by creating a
 * THEME_preprocess_page__story() preprocess function or page--story.vars.php
 * file. All the properties of the new hook is inherited from the base hook
 * and a new point of entry for variable modification is created for the
 * suggestion.
 */
function plex_theme_implicit_hook_registration(&$registry) {
  $hook_origins = plex_theme_get_hook_origins();
  $hook_fill    = array();

  if (hex_settings('plex_theme.implicit_hook_registration:from_processors', HEX_FIRST_PARENT)) {
    // Register hooks based on variable process functions and .vars.php files.
    // All user defined functions.
    list(, $user_func) = array_values(get_defined_functions());
    // Filtered for target themes and plug-ins.
    $active_components = implode('|', hex()->listAll());
    // All known base hooks. Discovered functions must be based on these.
    $hooks = implode('|', array_keys($hook_origins));

    // Find new hook suggestions derived from variable processor names.
    // Create a new entry by inheriting all the data from the closest parent hook.
    foreach (preg_grep("/^($active_components)_(pre)?process_($hooks)__/", $user_func) as $processor) {
      $theme_hook = substr($processor, strpos($processor, 'process_') + strlen('process_'));
      // Build the list only if it doesn't exist already. plex_theme_find_parent_hook()
      // makes sure the current $theme_hook doesn't exist before looking for the $parent_hook.
      if ($parent_hook = plex_theme_find_parent_hook($registry, $theme_hook)) {
        // Uses the immediate parent hook. Not necessarily the base hook.
        $hook_fill[$theme_hook] = $registry[$parent_hook];
        // Remove variable processors if this is a base hook. They are handled separately.
        if (isset($hook_origins[$parent_hook])) {
          foreach (array('preprocess functions', 'process functions') as $phase_process) {
            if (isset($hook_fill[$theme_hook][$phase_process])) {
              unset($hook_fill[$theme_hook][$phase_process]);
            }
          }
        }
        // Add the variable processor.
        $phase = strpos($processor, '_preprocess_') ? 'preprocess functions' : 'process functions';
        $hook_fill[$theme_hook][$phase][] = $processor;
      }
    }
  }

  if (hex_settings('plex_theme.implicit_hook_registration:from_vars_inc', HEX_FIRST_PARENT)) {
    // Find hooks derived from vars.php files. Same idea as above but the
    // include file is added later.
    foreach (plex_theme_get_includes() as $file) {
      if ($file->type === 'vars') {
        $theme_hook = str_replace('-', '_', $file->basename);
        if ($parent_hook = plex_theme_find_parent_hook($registry, $theme_hook)) {
          $hook_fill[$theme_hook] = $registry[$parent_hook];
          if (isset($hook_origins[$parent_hook])) {
            foreach (array('preprocess functions', 'process functions') as $phase_process) {
              if (isset($hook_fill[$theme_hook][$phase_process])) {
                unset($hook_fill[$theme_hook][$phase_process]);
              }
            }
          }
        }
      }
    }
  }

  $registry += $hook_fill;
}

/**
 * Collect .php or .inc files grouped by type.
 *
 * The types will be exposed as the following object members.
 *  - $includes->theme_inc
 *  - $includes->func_inc
 *  - $includes->vars_inc
 */
function plex_theme_get_includes_by_type($registry) {
  $theme_inc = array();
  $func_inc  = array();
  $vars_inc  = array();

  foreach (plex_theme_get_includes() as $file) {
    if ($file->type === 'theme') {
      $hooks = array();
      $host  = $file->host;
      foreach ($file->contents['function'] as $function) {
        $prefix = "{$host}_";
        $hook   = substr($function, strlen($prefix));
        if (strpos($function, $prefix) === 0 && isset($registry[$hook])) {
          $theme_inc[$hook][] = $file->uri;
        }
        else {
          foreach (array('preprocess', 'process') as $phase) {
            $prefix = "{$host}_{$phase}_";
            $hook   = substr($function, strlen($prefix));
            if (strpos($function, $prefix) === 0 && isset($registry[$hook])) {
              $theme_inc[$hook][] = $file->uri;
              break;
            }
          }
        }
      }
      // Make sure each hook contains unique include files.
      foreach ($theme_inc as $hook => $includes) {
        $theme_inc[$hook] = array_unique($includes);
      }
    }
    elseif ($file->type === 'func') {
      $hook = str_replace(array('-', '.func'), array('_', ''), $file->name);
      $func_inc[$hook][] = $file->uri;
    }
    elseif ($file->type === 'vars') {
      $named = str_replace(array('-', '.vars'), array('_', ''), $file->name);
      $vars_inc[$named][] = $file->uri;
    }
  }

  $includes = new stdClass();
  $includes->theme_inc = $theme_inc;
  $includes->func_inc  = $func_inc;
  $includes->vars_inc  = $vars_inc;

  return $includes;
}

/**
 * Scans for files needed for the theme registry.
 */
function plex_theme_get_includes() {
  $includes = &hex_static(__FUNCTION__)->data;

  if (!isset($includes)) {
    $includes = array();

    $lod_theme_inc = hex_settings('plex_theme.load_on_demand:theme_inc', HEX_FIRST_PARENT);
    $lod_func_inc  = hex_settings('plex_theme.load_on_demand:func_inc' , HEX_FIRST_PARENT);
    $lod_vars_inc  = hex_settings('plex_theme.load_on_demand:vars_inc' , HEX_FIRST_PARENT);

    // Start by creating a filter so only valid files are returned.
    $hooks = $origins = array();
    foreach (plex_theme_get_hook_origins() as $hook => $origin) {
      $hooks[] = $hook;
      if (!in_array($origin, $origins)) {
        $origins[] = $origin;
      }
    }

    $origin_regex = '/^(' . implode('|', $origins) . ')$/';
    $hook_regex   = '/^(' . str_replace('_', '-', implode('|', $hooks)) . ')(--[[:alnum:]-]+)?$/';

    $files = new HexContentScanner();
    $files->scan('/^[[:alpha:]][[:alnum:]._-]+\.(theme|func|vars)\.(inc|php)$/', 'all');
    foreach ($files as $file) {
      $parts = explode('.', $file->name);
      $name  = array_shift($parts);
      $type  = array_pop($parts);

      if ($type === 'theme' && $lod_theme_inc && preg_match($origin_regex, $name)
      ||  $type === 'func'  && $lod_func_inc  && preg_match($hook_regex, $name)
      ||  $type === 'vars'  && $lod_vars_inc  && preg_match($hook_regex, $name)) {
        $file->basename = $name;
        $file->type     = $type;
        $includes[] = $file;
      }
    }
  }

  return $includes;
}

/**
 * Returns all theming hooks and their origins.
 */
function plex_theme_get_hook_origins() {
  $hook_origins = &hex_static(__FUNCTION__)->data;

  if (!isset($hook_origins)) {
    $hook_origins = array();

    // False pass of hook_theme() so the theming hooks implemented by each
    // component can be determined. $theme_hooks does nothing. This is only
    // to prevent potential notices or errors.
    $theme_hooks = array();
    $components = hex()->attach(new HexModules())->listType('module');
    foreach (hex()->listType('theme') as $theme) {
      $components += hex()->listType('plugin', $theme) + array($theme => $theme);
    }
    foreach ($components as $component) {
      // Determine the $type that's usually passed to hook_theme().
      $type = hex()->type($component);
      switch ($type) {
        case 'theme' :
          $type = $component === HEX_ACTIVE_THEME ? 'theme' : 'base_theme';
          break;
        case 'plugin' :
          $host = hex()->info('host', $component);
          $type = $host === HEX_ACTIVE_THEME ? 'theme_plugin' : 'base_theme_plugin';
          break;
      }
      $hook_theme = "{$component}_theme";
      if (hex_function_exists($hook_theme)) {
        $result = $hook_theme($theme_hooks, $type, $component, hex()->path($component));
        foreach (array_keys($result) as $hook) {
          // Maintain first origin.
          if (!isset($hook_origins[$hook])) {
            $hook_origins[$hook] = $component;
          }
        }
        $theme_hooks += $result;
      }
    }
  }

  return $hook_origins;
}

/**
 * Looks for the parent hook when given a hook based on a pattern.
 *
 * @param $hooks
 *  The set of theming hooks to check against.
 * @param $hook_pattern
 *  The hook pattern being processed.
 */
function plex_theme_find_parent_hook($hooks, $hook_pattern) {
  $_hook = $hook_pattern;

  if (!isset($hooks[$hook_pattern])) {
    while ($pos = strrpos($_hook, '__')) {
      $_hook = substr($_hook, 0, $pos);
      if (isset($hooks[$_hook])) {
        break;
      }
    }
  }

  return $_hook !== $hook_pattern ? $_hook : FALSE;
}
