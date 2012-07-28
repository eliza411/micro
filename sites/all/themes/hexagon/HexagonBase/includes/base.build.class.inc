<?php

/**
 * For scanning plug-ins.
 */
class HexPluginScanner extends HexFileScanner {

  /**
   * Scans for all plug-ins in the file system.
   */
  public function scanAll() {
    parent::scan('/^' . DRUPAL_PHP_FUNCTION_PATTERN . '\.pinfo$/', 'theme');
    return $this;
  }

  /**
   * Provides scanning contexts for the given $scope.
   */
  protected function scanningContexts($scope) {
    $contexts = &hex_static(__METHOD__)->data["$scope:{$this->offsetStart}-{$this->offsetEnd}"];
    if (!isset($contexts)) {
      $contexts = array();
      foreach (hex()->listType('theme') as $theme) {
        if (in_array($scope, array($theme, 'theme', 'all'))) {
          $context         = new stdClass();
          $context->host   = $theme;
          $context->hosts  = array($theme);
          $context->dir    = hex()->path($theme);
          $context->ignore = implode('|', array_map('preg_quote', preg_grep('/^' . preg_quote(hex()->path($theme), '/') . '.+/', $this->getIgnored())));
          $contexts[]      = $context;
        }
      }
    }
    return $contexts;
  }

  /**
   * Returns a list of paths to ignore.
   */
  protected function getIgnored() {
    // 'ignore' holds the paths to *all* themes even if they are not active.
    $ignore = array();
    foreach (list_themes() as $theme) {
      $ignore[] = dirname($theme->filename);
    }
    return $ignore;
  }

}

/**
 * For building plug-ins.
 */
class HexBuildPlugins extends ArrayObject {
  protected $theme;
  protected $plugins;
  protected $errors;
  protected $status;
  protected $known;
  protected $enablers;

  /**
   * Initialize.
   */
  public function __construct($theme) {
    $this->theme = $theme;
    $this->buildPlugins();
    $plugins = array();
    if ($this->plugins->count()) {
      $sort = array();
      foreach ($this->plugins as $plugin) {
        $plugins[$plugin->name] = $plugin;
        $sort[$plugin->name]    = $plugin->sort;
      }
      // Positive values float. Negative drops.
      arsort($sort);
      // Merge takes order from $sort and fills with plug-in objects.
      $plugins = array_merge($sort, $plugins);
    }
    $this->exchangeArray($plugins);
  }

  /**
   * Returns enabled plug-ins.
   */
  public function getEnabledCopy() {
    $this->buildEnablers();
    $plugins = array();
    foreach ($this as $name => $plugin) {
      if ($this->isEnabled($name)) {
        $plugins[$name] = $plugin;
      }
    }
    return $plugins;
  }

  /**
   * Determines if the plug-in exists.
   *
   * @param $plugin
   *  The plug-in object or key (machine name).
   */
  public function exists($plugin) {
    return is_object($plugin) ? $this->plugins->contains($plugin) : $this->getExisting($plugin) !== NULL;
  }

  /**
   * Determines if the plug-in contains errors.
   *
   * @param $plugin
   *  The plug-in object or key (machine name).
   */
  public function hasErrors($plugin) {
    // If a string is passed, make sure it's not part of $this->plugins which
    // are assumed to be valid.
    if (is_string($plugin) && $this->getExisting($plugin) === NULL) {
      foreach ($this->errors as $error) {
        if ($error->name === $plugin) $plugin = $error; break;
      }
    }
    return is_object($plugin) ? $this->errors->contains($plugin) : FALSE;
  }

  /**
   * Returns all errors for a given plug-in $plugin.
   */
  public function getErrors($plugin) {
    if ($plugin === (object) $plugin) $plugin = $plugin->name;
    $errors = array();
    foreach ($this->status as $status) {
      if ($status->plugin->name === $plugin && $status->severity === E_USER_ERROR) {
        $this->status->detach($status);
        $errors[] = $status;
      }
    }
    return $errors;
  }

  /**
   * Displays errors for enabled plug-ins.
   */
  public function displayErrors() {
    foreach ($this->getEnabled() as $plugin) {
      if ($this->hasErrors($plugin)) {
        $errors = $this->getErrors($plugin);
        trigger_error(
          t('%plugin could not be enabled. It contains the following errors:',
            array('%plugin' => end($errors)->plugin->info['name'])
          ),
          E_USER_ERROR
        );
        foreach ($errors as $error) {
          trigger_error(t($error->message, $error->variables), $error->severity);
        }
        unset($this->enablers[$plugin]);
      }
      elseif (!$this->exists($plugin)) {
        trigger_error(t('%plugin could not be enabled. It does not exist.', array('%plugin' => $plugin)), E_USER_ERROR);
        unset($this->enablers[$plugin]);
      }
    }
  }

  /**
   * Generates plug-ins data.
   */
  protected function buildPlugins() {
    hex()->attach(new HexModules());

    $this->plugins     = new SplObjectStorage();
    $this->errors      = new SplObjectStorage();
    $this->status      = new SplObjectStorage();
    $this->known       = variable_get('hex_plugins:' . $this->theme, array());
    $this->accumulator = array();

    $scanner = new HexPluginScanner();
    $scanner->offsetEnd($this->theme);

    foreach ($scanner->scanAll() as $file) {
      // $plugin will inherit the following:
      //  - $plugin->uri
      //  - $plugin->filename
      //  - $plugin->name
      //  - $plugin->host
      //  - $plugin->dir
      $plugin = clone $file;
      // Parse .pinfo file and append defaults.
      $plugin->info = drupal_parse_info_file($plugin->uri) + array(
        'name'         => 'Untitled',
        'description'  => '',
        'version'      => NULL,
        'hexapi'       => NULL,
        'php'          => PHP_VERSION,
        'protected'    => FALSE,
        'auto enable'  => FALSE,
        'weight'       => 0,
        'dependencies' => array(),
        'requirements' => array(),
        'stylesheets'  => array(),
        'scripts'      => array(),
        'settings'     => array(),
      );
      $this->plugins->attach($plugin);
    }
    // Validate.
    $this->validateVersions();
    $this->validateDuplicates();
    $this->validateOverrides();

    // Set and check dependencies but do it incrementally. This ensures base
    // theme hosted plug-ins are locked out from setting sub-theme hosted
    // plug-ins as dependencies. Sub-theme hosted plug-ins can depend on base
    // theme hosted plug-ins, not the other way around.
    $plugins_by_host = array();
    $accumulator     = array();
    foreach ($this->plugins as $plugin) {
      $plugins_by_host[$plugin->host][$plugin->name] = $plugin;
    }
    foreach ($plugins_by_host as $host => $host_plugins) {
      // Don't allow accumulator to modify the plug-in directly. Use clone and
      // pick out the properties required from the modified clone.
      foreach ($host_plugins as $plugin) {
        $accumulator[$plugin->name] = clone $plugin;
      }
      // Incrementally check for dependencies.
      // This core function is designed for modules but it's generic enough to
      // use for plug-in's. It builds dependency data any levels deep. Three new
      // object members, "requires", "required_by" and "sort" will be added.
      $accumulator = _module_build_dependencies($accumulator);
      // Set weight and properties from accumulator. Weight based on current
      // host level, accumulator and .info defined weight.
      foreach ($host_plugins as $key => $plugin) {
        $plugin->requires    = array_keys($accumulator[$key]->requires);
        $plugin->required_by = array_keys($accumulator[$key]->required_by);
        $plugin->sort        = $accumulator[$key]->sort + $plugin->info['weight'];
      }
      // Validate plug-in dependencies.
      foreach ($host_plugins as $plugin) {
        $this->validateDependencies($plugin, $accumulator);
      }
    }

    // Process and expose dependencies of all other types.
    foreach ($this->plugins as $plugin) {
      // Remove dependents if they contain errors.
      foreach ($plugin->required_by as $i => $required_by) {
        if ($this->hasErrors($required_by)) {
          unset($plugin->required_by[$i]);
        }
      }

      // Plug-ins that depend on this plug-in.
      $plugin->info['required by plugin'] = $plugin->required_by;
      // 'requires' will contain all requirements.
      $plugin->info['required']        = $plugin->requires;
      $plugin->info['required plugin'] = $plugin->requires;
      foreach (hex()->listTypes() as $type) {
        if ($type === 'plugin') continue;
        $plugin->info["required $type"] = array();
      }

      // $plugin->requires contains dependent plug-ins in multiple depths.
      // This is used to process requirements for modules and themes in
      // multiple depths.
      $requirements = array();
      foreach (array_merge(array($plugin->name), $plugin->requires) as $key) {
        $requirements = array_merge($requirements, (array) $this->getExisting($key)->info['requirements']);
      }
      foreach ($requirements as $requires) {
        // All requirements.
        $plugin->info['required'][] = $requires;
        if (!hex()->exists($requires)) {
          $plugin->info["required missing"][]  = $requires;;
        }
        elseif ($type = hex()->type($requires)) {
          // Requirements by type.
          $plugin->info["required $type"][]  = $requires;
        }
      }
    }

    return $this;
  }

  /**
   * Returns active plug-ins.
   */
  protected function buildEnablers() {
    $this->enablers = array();
    /*
      TODO Base themes enabling sub-theme hosted plug-ins should generate an error.
    */
    // Theme .info enabled.
    foreach (hex()->listType('theme') as $theme) {
      foreach ((array) hex()->info('hex plugins', $theme) as $name) {
        $this->setEnabled($name, $theme);
      }
      // Only up to the theme being processed.
      if ($theme === $this->theme) break;
    }

    // Auto enabled.
    foreach ($this->getArrayCopy() as $plugin) {
      if (hex_auto_type($plugin->info['auto enable'])) {
        if (empty($plugin->info['required missing'])) {
          $this->setEnabled($plugin->name, '#auto');
        }
      }
    }

    // Feature toggle enabled.
    foreach (hex_base_generate('features') as $feature) {
      if ($this->offsetExists($feature->host) && $feature->value) {
        $this->setEnabled($feature->host, '#feature');
      }
    }
    return $this;
  }

  /**
   * Validate against PHP_VERSION and HEX_API versions.
   */
  protected function validateVersions() {
    for ($this->plugins->rewind(); $this->plugins->valid(); $this->plugins->next()) {
      $plugin = $this->plugins->current();
      // PHP compatibility.
      if (version_compare(PHP_VERSION, $plugin->info['php']) < 0) {
        $this->setError(
          $plugin,
          '%plugin requires PHP version %plugin.php. Version %server.php installed on this server is not compatible.',
          array('%plugin'     => $plugin->info['name'],
                '%plugin.php' => $plugin->info['php'],
                '%server.php' => PHP_VERSION)
        );
      }
      // Base theme api compatibility.
      if (!$plugin->info['hexapi']) {
        $this->setError(
          $plugin,
          '%plugin has not defined the hexapi version compatibility. It must be written for version %hexapi and defined in the plug-in info file.',
          array('%plugin' => $plugin->info['name'],
                '%hexapi' => HEX_API)
        );
      }
      elseif (HEX_API !== $plugin->info['hexapi']) {
        $this->setError(
          $plugin,
          '%plugin was not written for Hexagon API version %hexapi.',
          array('%plugin' => $plugin->info['name'],
                '%hexapi' => HEX_API)
        );
      }
    }
    return $this;
  }

  /**
   * Prevents duplicate plug-ins hosted in the same theme.
   */
  protected function validateDuplicates() {
    // Must compare with a clone so this loops independently of each other.
    // The contents will be referencing the same data.
    $storage = clone $this->plugins;
    for ($this->plugins->rewind(); $this->plugins->valid(); $this->plugins->next()) {
      $plugin           = $this->plugins->current();
      $has_duplicates   = FALSE;
      $known_exists     = FALSE;
      $current_is_known = isset($this->known[hash('md5', $plugin->uri)]);
      $plugin_count     = 0;
      foreach ($storage as $stored) {
        if ($stored->name === $plugin->name && $stored->host === $plugin->host) {
          if (!$known_exists && isset($this->known[hash('md5', $stored->uri)])) $known_exists = TRUE;
          if ($plugin_count++) $has_duplicates = TRUE;
          if ($known_exists && $has_duplicates) break;
        }
      }
      if ($has_duplicates && !$known_exists || $known_exists && !$current_is_known) {
        $this->setError(
          $plugin,
          '%plugin found in %host is a duplicate. Please remove %path.',
          array('%plugin' => $plugin->info['name'],
                '%host'   => hex()->info('name', $plugin->host),
                '%path'   => str_replace(hex()->path($plugin->host), '', $plugin->uri))
        );
        $storage->detach($plugin);
      }
    }
    return $this;
  }

  /**
   * Prevents duplicate plug-ins hosted in the same theme.
   *
   * Validating duplicates must come before this.
   */
  protected function validateOverrides() {
    // Must compare with a clone so this loops independently of each other.
    // The contents will be referencing the same data.
    $storage = clone $this->plugins;
    for ($this->plugins->rewind(); $this->plugins->valid(); $this->plugins->next()) {
      $plugin            = $this->plugins->current();
      $was_overridden    = FALSE;
      $is_override       = FALSE;
      $base_is_protected = FALSE;
      $overrides = array();
      // Get hosts (themes) and protected state.
      foreach ($storage as $stored) {
        if ($stored->name === $plugin->name) $overrides[$stored->host] = $stored->info['protected'];
      }
      $level  = 0;
      $levels = count($overrides);
      $locked = FALSE;
      foreach ($overrides as $host => $protected) {
        ++$level;
        if ($host === $plugin->host) {
          $was_overridden    = $level === 1 && $level < $levels && !$protected;
          $is_override       = $level > 1;
          $base_is_protected = $locked;
        }
        // Hold locked state.
        if (!$locked && $protected) $locked = TRUE;
      }
      // Remove if it was overridden.
      if ($was_overridden) {
        $this->plugins->detach($plugin);
        $storage->detach($plugin);
      }
      elseif ($base_is_protected && $is_override) {
        $this->setError(
          $plugin,
          '%plugin is protected. The override found in %theme is not permitted.',
          array('%plugin' => $duplicate->info['name'],
                '%host'   => hex()->info('name', $host),
                '%path'   => str_replace(hex()->path($host), '', $plugin->uri))
        );
        $storage->detach($plugin);
      }
      
    }
    return $this;
  }

  /**
   * Validates dependencies.
   */
  protected function validateDependencies($plugin, $current_set) {
    foreach ($plugin->requires as $key) {
      // Check for dependencies found so far. Plug-ins within base themes
      // pointing to other plug-ins hosted in a sub-theme is not be allowed.
      if ($this->exists($key) && !isset($current_set[$key])) {
        $this->setError(
          $plugin,
          '%plugin depends on %dependency which is hosted in a sub-theme. This is not a valid use of dependencies.',
          array('%plugin'     => $plugin->info['name'],
                '%dependency' => $this->getExisting($key)->info['name'])
        );
      }
      // Catch previously detected errors.
      elseif ($this->hasErrors($key)) {
        $this->setError(
          $plugin,
          '%plugin depends on %dependency which contains errors.',
          array(
            '%plugin'     => $plugin->info['name'],
            '%dependency' => $dependency->info['name'],
          )
        );
      }
      // Completely missing.
      elseif (!$this->exists($key)) {
        $this->setError(
          $plugin,
          '%plugin depends on %dependency which cannot be found.',
          array(
            '%plugin'     => $plugin->info['name'],
            '%dependency' => $key,
          )
        );
      }
    }
  }

  /**
   * Returns an existing, valid plug-in by name.
   * 
   * @param $name
   *  The plug-in name.
   */
  protected function getExisting($name, $source = 'plugins') {
    foreach ($this->$source as $plugin) if ($plugin->name === $name) return $plugin;
  }

  /**
   * Sets an error.
   *
   * This will detach the plug-in from $this->plugins and move it to
   * $this->errors.
   *
   * @see format_string()
   *
   * @param $plugin
   *  The plug-in object.
   * @param $message
   *  The error message.
   * @param $variables
   *  Variables for the error message.
   */
  protected function setError($plugin, $message, $variables = array()) {
    $this->plugins->detach($plugin);
    $this->errors->attach($plugin);
    $this->status->attach((object) array(
      'plugin'    => $plugin,
      'message'   => $message,
      'variables' => $variables,
      'severity'  => E_USER_ERROR,
    ));
    return $this;
  }

  /**
   * Set enabled state for the plug-in.
   * 
   * @param $name
   *  Plug-in name.
   * @param $enabler
   *  The enabler. Can be a theme key, #auto or #feature.
   */
  protected function setEnabled($name, $enabler) {
    // Forced disable if prefixed with a bang "!". This will carry over
    // into sub-themes. This is generally not needed but when a theme
    // needs to explictly disable a plug-in, it can do it.
    $enable = $name[0] !== '!';
    $name   = ltrim($name, "! \t");
    $this->enablers[$name][$enabler] = $enable;
    // Also enable dependencies.
    if ($this->exists($name) && $enable) {
      foreach ($this->getExisting($name)->info['required plugin'] as $required) {
        $this->enablers[$required][$enabler] = TRUE;
      }
    }
  }

  /**
   * Get enabled plug-in keys.
   */
  protected function getEnabled() {
    $enabled = array();
    foreach (array_keys((array) $this->enablers) as $plugin) {
      if ($this->isEnabled($plugin)) $enabled[] = $plugin;
    }
    return $enabled;
  }

  /**
   * Returns the enabled state for the plug-in $name.
   */
  protected function isEnabled($name) {
    $enabled = FALSE;
    if (isset($this->enablers[$name])) {
      foreach ($this->enablers[$name] as $info) {
        // Defaults to true but if any of the enablers disables the feature then
        // it will be disabled.
        $enabled = TRUE;
        foreach ((array) $info as $enabler => $enable) {
          if ($enabled && !$enable) $enabled = FALSE;
        }
      }
    }
    return $enabled;
  }

}

/**
 * For building features.
 */
class HexBuildFeatures extends ArrayObject {
  protected $theme;
  protected $settings;
  protected $supported;
  protected $weights;

  /**
   * Initialize.
   */
  public function __construct($theme) {
    $this->theme = $theme;
    $build = hex_base_generate('settings', $this->theme);
    foreach ($build as $key => $setting) {
      $this->settings[$build->fapiKey($key)] = $setting;
    }

    // If the theme defines 'features' through the .info file, limit the
    // supported features to what has been defined. If nothing was set, all
    // features are supported. This follows the behavior in Drupal core.

    // The info file is parsed since Drupal core adds the default set of
    // core theme features with _system_rebuild_theme_data(). This prevents
    // hex()->info() from detecting whether the theme defined the default
    // system features vs. core adding them in by default. If nothing was
    // set, all extra features will be supported.
    $info = drupal_parse_info_file(hex()->path($this->theme) . "/{$this->theme}.info");
    $this->supported = &$info['features'];
    // Weights dererived from the order they are defined from .info.
    $this->weights = isset($this->supported) ? array_flip($this->supported) : array();
    // Build.
    $this->buildCore()->buildHex();
  }

  /**
   * Returns an array of toggle settings. Used for building settings.
   *
   * @see hex_settings_build()
   */
  public function getToggleSettings() {
    $features = array();
    foreach ($this as $feature) {
      $features[$feature->toggleKey] = $feature->value;
    }
    return $features;
  }

  /**
   * Determines if a feature is supported or not.
   *
   * Supported state depends on the feature being selectively defined from
   * the theme's .info file. If nothing is defined, all features will be
   * exposed in the theme settings form and considered supported.
   *
   * @param $feature
   *  The feature key or object.
   */
  public function isSupported($feature) {
    if ($feature === (object) $feature) $feature = $feature->key;
    return empty($this->supported) || in_array($feature, $this->supported);
  }

  /**
   * Builds core features.
   *
   * @see system_theme_settings()
   */
  protected function buildCore() {
    // Default system feature toggles.
    $defaults = array(
      'logo'                      => t('Logo'), 
      'name'                      => t('Site name'), 
      'slogan'                    => t('Site slogan'), 
      'node_user_picture'         => t('User pictures in posts'), 
      'comment_user_picture'      => t('User pictures in comments'), 
      'comment_user_verification' => t('User verification status in comments'), 
      'favicon'                   => t('Shortcut icon'), 
      'main_menu'                 => t('Main menu'), 
      'secondary_menu'            => t('Secondary menu'),
    );
    // Some features are not always available
    $disabled = array();
    if (!variable_get('user_pictures', 0)) {
      $disabled['toggle_node_user_picture']    = TRUE;
      $disabled['toggle_comment_user_picture'] = TRUE;
    }
    if (!module_exists('comment')) {
      $disabled['toggle_comment_user_picture']      = TRUE;
      $disabled['toggle_comment_user_verification'] = TRUE;
    }
    foreach ($defaults as $key => $title) {
      $this->add(
        $key,          // feature key
        "toggle_$key", // toggle key
        $title,        // feature title
        'system',      // host key
        'module',      // host type
        'System',      // host name
        !empty($disabled["toggle_$key"]) // disabled state
      );
    }
    return $this;
  }

  /**
   * Builds Hex based theme and plug-in features.
   */
  protected function buildHex() {
    // Collect feature sets from all themes and plug-ins.
    $plugins = hex_base_generate('plugins', $this->theme);
    $delimit = HexBuildSettings::DELIMIT_NAMESPACE;
    foreach (hex()->listType('theme') as $theme_key) {
      foreach ((array) hex()->info('feature set', $theme_key) as $key => $title) {
        $this->add(
          $key,          // feature key
          "toggle_$key", // toggle key
          $title,        // feature title
          $theme_key,    // host key
          'theme',       // host type
          hex()->info('name', $theme_key) // host name
        );
      }
      foreach ($plugins as $plugin_key => $plugin) {
        if ($plugin->host === $theme_key && isset($plugin->info['feature set'])) {
          foreach ($plugin->info['feature set'] as $key => $title) {
            $toggle_key = "$plugin_key{$delimit}toggle_$key";
            $this->add(
              $key,                  // feature key
              $toggle_key,           // toggle key
              $title,                // feature title
              $plugin_key,           // host key
              'plugin',              // host type
              $plugin->info['name'], // host name
              $plugins->hasErrors($plugin) // disabled state.
            );
          }
        }
      }
    }
    return $this;
  }

  /**
   * Add to the pool of features.
   *
   * @param $key
   *  A unique key identifying the feature.
   * @param $toggle
   *  The toggle key used in the theme settings form.
   * @param $title
   *  The human readable feature title used in the theme settings form.
   * @param $host_type
   *  The host that implemented the feature. It can be 'module' for the core
   *  defaults, 'theme' or 'plugin'.
   * @param $host_name
   *  Human readable host name.
   * @param $disabled
   *  Optionally sets the disabled state. This will be set to the '#disabled'
   *  setting in the theme settings form.
   */
  protected function add($key, $toggle, $title, $host, $host_type, $host_name, $disabled = FALSE) {
    // The first encountered host that defines a feature will stick.
    // Overriding is not allowed.
    if (!$this->offsetExists($key)) {
      $weight  = &$this->weights[$key];
      $feature = new stdClass();
      $feature->key       = $key;
      $feature->toggleKey = $toggle;
      $feature->title     = $title;
      $feature->host      = $host;
      $feature->hostType  = $host_type;
      $feature->hostName  = $host_name;
      $feature->weight    = isset($weight) ? $weight : count($this->weights);
      if ($this->isSupported($key)) {
        $feature->value = (int) (isset($this->settings[$toggle]) && $this->settings[$toggle]);
      }
      else {
        $feature->value = NULL;
      }
      $feature->disabled  = $disabled;
      $this->offsetSet($key, $feature);
    }
    else {
      trigger_error(
        t('The feature %feature.title (%feature.key) defined by %host.invalid is not allowed. It was already defined by %host.',
          array('%feature.title' => $title,
                '%feature.key'   => $key,
                '%host.invalid'  => $host_name,
                '%host'          => $this->features[$key]->hostName)
        ),
        E_USER_WARNING
      );
    }
  }

}

/**
 * Tools for building settings.
 */
class HexBuildSettings extends HexNameSpaces {
  const DELIMIT_NAMESPACE   = '__NS__';
  const DELIMIT_CONDITIONAL = '__PC__';
  const FALLBACK            = 'FALLBACK__';
  protected $theme;

  /**
   * Initialize.
   */
  public function __construct($theme) {
    $this->theme     = $theme;
    $this->delimiter = '.';
    $this->system()->defaults()->saved();
    foreach ($this->exchangeArray(array()) as $key => $setting) {
      $this->offsetSet($this->fapiKey($key, TRUE), $setting);
    }
  }

  /**
   * Returns a list of parents.
   */
  public function getParents($setting_key) {
    $parents = array();
    $parent  = str_replace(self::FALLBACK, '', $setting_key);
    while (($pos = strrpos($parent, ':')) !== FALSE) {
      $parent = substr($parent, 0, $pos);
      $parents[] = $parent;
    }
    return $parents;
  }

  /**
   * Converts form api array keys so they are compatible with namespaces and
   * parent conditional settings.
   *
   * Forms do not accept dots and colons for the keys.
   */
  public function fapiKey($key, $revert = FALSE) {
    switch ($revert) {
      // Dot seen as an array namespace and colon for parent conditionals.
      case FALSE : return str_replace(array('.', ':'), array(self::DELIMIT_NAMESPACE, self::DELIMIT_CONDITIONAL), $key);
      case TRUE  : return str_replace(array(self::DELIMIT_NAMESPACE, self::DELIMIT_CONDITIONAL), array('.', ':'), $key);
    }
  }

  /**
   * Builds saved settings.
   */
  protected function saved() {
    // Global settings used as pass-through defaults.
    $global = variable_get('theme_settings', array());
    $this->add(variable_get("theme_{$this->theme}_settings", $global));
    return $this;
  }

  /**
   * Builds default info settings.
   */
  protected function defaults() {
    $this->setValueCallback('hex_auto_type');
    // Plug-in settings are added first so themes can override.
    $plugins = hex_base_generate('plugins', $this->theme);
    foreach ($plugins as $plugin) {
      if ($plugin->host === $this->theme && $plugin->info['settings']) {
        // Prefixes the plug-in name for plug-in defined settings.
        $this->add(array(self::FALLBACK . $plugin->name => $plugin->info['settings']));
      }
    }
    if ($settings = hex()->info('settings', $this->theme)) {
      $this->add($settings);
    }
    $this->unsetValueCallback();
    return $this;
  }

  /**
   * Builds drupal core theme settings.
   */
  protected function system() {
    $system = array();
    // Get defaults from the core theme_get_setting() function.
    foreach (array('default_logo',
                   'logo_path',
                   'logo',
                   'default_favicon',
                   'favicon_path',
                   'favicon_mimetype',
                   'favicon') as $key) {
      $system[$key] = theme_get_setting($key, $this->theme);
    }
    $this->add($system);
    return $this;
  }

}

/**
 * For building the code registry.
 */
class HexBuildRegistry extends HexContentScanner {

  /**
   * Initialize.
   */
  public function __construct($theme = NULL) {
    if (!isset($theme)) $theme = hex()->theme();
    if (hex()->validate($theme, __FUNCTION__, 'theme')) {
      $this->theme = $theme;
      $this->buildRegistry();
    }
  }

  /**
   * Builds the code registry.
   */
  protected function buildRegistry() {
    $scanned = array();
    foreach (hex()->listType('theme') as $theme) {
      // Collect all info defined code categories before scanning.
      $categories = (array) hex()->info('hex code categories', $theme);
      foreach (hex()->listType('plugin', $theme) as $plugin) {
        $categories = array_merge($categories, (array) hex()->info('hex code categories', $plugin));
      }
      // Scanning can only start from the current theme level. This prevents
      // sub-theme defined categories from scanning it's parents.
      $this->offsetStart($theme);
      $mask = implode('|', array_map('preg_quote', array_diff($categories, $scanned)));
      $this->scan("/\.($mask)\.(inc|php)$/", 'all');
      $scanned = array_merge($scanned, $categories);
      // Don't go beyond the build target.
      if ($theme === $this->theme) break;
    }

    $registry = array();
    $files = $this->exchangeArray(array());
    foreach ($files as $file) {
      $hash     = hash_file('sha256', $file->uri);
      $contents = array();
      if (!isset($registry['includes'][$hash])) {
        foreach ($file->contents as $type => $items) {
          foreach ($items as $item) {
            $contents[$type][$item] = $hash;
          }
        }
        if ($contents) {
          $include = new stdClass();
          $include->uri        = $file->uri;
          $include->filename   = $file->filename;
          $include->host       = $file->host;
          $include->categories = $this->resolveCategories($file);
          $registry['includes'][$hash] = $include;
          $registry = array_merge_recursive($registry, $contents);
        }
      }
    }
    $this->exchangeArray($registry);

    return $this;
  }
  
  /**
   * Determines the file category.
   */
  protected function resolveCategories($file) {
    $categories = array();
    if (strpos($file->name, '.') !== FALSE) {
      $namespace = $segments = explode('.', "{$file->host}.{$file->name}");
      while (list($i, $segment) = each($segments)) {
        $categories[implode('.', array_reverse($namespace))] = $i;
        unset($namespace[$i]);
      }
    }
    return $categories;
  }
  
}
