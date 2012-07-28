<?php

/**
 * Returns the path to the most relevant file by passing in its name.
 *
 * The file returned will be closest to the active theme. This is important to
 * understand when the active theme has a parent or multiple parents and/or
 * plug-ins. It will fallback in this order:
 *
 * - active theme
 * - plug-in hosted by the active theme
 * - parent theme
 * - plug-in hosted by the parent theme
 * - etc...
 *
 * This also depends on the scope and namespace of the defined file extensions
 * and patterns from the .info or .pinfo file.
 *
 *  plex file extensions[SCOPE][NAMESPACE] = .ext
 *  plex file patterns[SCOPE][NAMESPACE]   = /^regex$/
 *
 * When a namespace is not defined, it is in the virtual root.
 *
 *  plex file extensions[SCOPE][] = .css
 *  plex file extensions[SCOPE][] = .js
 *  plex file patterns[SCOPE][]   = /^regex$/
 *
 * The scope defines where the search occurs. Valid values are "all", "theme",
 * "plugin", a specific theme or plugin key.
 *
 *  plex file extensions[all][NAMESPACE]  = .ext
 *  plex file patterns[theme][NAMESPACE]  = /^regex$/
 *  plex file patterns[plugin][NAMESPACE] = /^regex$/
 *  plex file patterns[my_theme][visual]  = /^(color|layout|type)\.css$/
 *
 * File patterns can also be set from a hook supplied by this plug-in. The same
 * rules apply as setting them from info files except that it only works with
 * regex patterns:
 *
 *  function [THEME_NAME|PLUGIN_NAME]_plex_file_patterns() {
 *    return array(
 *      'SCOPE' => array(
 *        'NAMESPACE' => '/^regex$/',
 *      ),
 *    );
 *  }
 *
 * Note: If a file or directory is prefixed with a ~tilde, it will be ignored.
 * This may be useful when you need to disable the file or group of files
 * temporarily.
 *
 * @param $namespace
 *  The file name including the namespace prefix.
 */
function plex_file_path($namespace) {
  $files = &hex_cache('plex_files')->data[$namespace];
  if ($files) return end($files)->uri;
}

/**
 * Returns an array of file paths for a given name.
 *
 * @param $namespace
 *  The file name including the namespace prefix.
 */
function plex_file_path_multiple($namespace) {
  $files = &hex_cache('plex_files')->data[$namespace];
  return array_keys($files);
}
