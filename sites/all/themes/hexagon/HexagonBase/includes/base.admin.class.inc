<?php

/**
 * Alters the theming context.
 */
class HexThemeContextSetter {
  const SET_ACTIVE   = 10;
  const SET_KEY      =  1;
  const SET_PREVIOUS = -1;

  protected $context;
  protected $stack;

  /**
   * Initialize
   */
  public function __construct($context) {
    $this->context = $context;
    $this->stack = array($context->theme);
  }

  /**
   * Sets the theme context to the active theme.
   */
  public function active() {
    $this->set(self::SET_ACTIVE);
    return $this;
  }

  /**
   * Sets the theme context to the theme responsible for the current form.
   *
   * @see hex_form_system_theme_settings_alter()
   */
  public function themeForm() {
    $this->set(self::SET_KEY);
    return $this;
  }

  /**
   * Reverts the theme context.
   *
   * The context must be changed before it can be reverted.
   */
  public function revert() {
    $this->set(self::SET_PREVIOUS);
    return $this;
  }

  /**
   * Sets the theme context.
   *
   * @param $set
   *  This sets the context which can be one of the following:
   *  - HexThemeContext::SET_ACTIVE
   *    This sets it to the active theme.
   *  - HexThemeContext::SET_KEY
   *    This sets it to the theme Drupal core is focused on. It is only relevant
   *    inside the theme settings form where core switches the theme key to set
   *    focus to the theme responsible for the settings form.
   *  - HexThemeContext::SET_PREVIOUS
   *    This resets the theme context. If a context is set, there must be a
   *    corresponding call to revert it.
   */
  protected function set($set) {
    switch ($set) {
      case self::SET_ACTIVE   : array_unshift($this->stack, HEX_ACTIVE_THEME);          break;
      case self::SET_KEY      : array_unshift($this->stack, $GLOBALS['theme_key']);     break;
      case self::SET_PREVIOUS : if (count($this->stack) > 1) array_shift($this->stack); break;
    }
    if ($this->context->theme !== $this->stack[0]) {
      $this->context->theme = $this->stack[0];
      hex_static_reset();
    }
    return $this;
  }

}

/**
 * Hex component for modules.
 */
class HexModules implements HexComponentsInterface {

  /**
   * Returns the component type.
   */
  public function type() {
    return 'module';
  }

  /**
   * Returns a list of modules.
   */
  public function listAll() {
    return module_list();
  }

  /**
   * Return the path for the given $name.
   */
  public function getPath($name) {
    return drupal_get_path('module', $name);
  }

  /**
   * Return info data for the given $name.
   */
  public function getInfo($name) {
    return system_get_info('module', $name);
  }

}

/**
 * Builds data and info caches.
 *
 * @ingroup hex_cache
 */
class HexCacheBuild {
  protected $cacheKey;
  protected $info;

  /**
   * Initialize.
   */
  public function __construct($cache_key) {
    $this->cacheKey = $cache_key;
  }

  /**
   * Builds data and info caches.
   */
  public function rebuild($iid, $cid) {
    $this->rebuildInfo($iid)->rebuildData($cid);
    return $this;
  }

  /**
   * Rebuilds the info cache.
   */
  protected function rebuildInfo($iid) {
    $info_all = &cache_get($iid, 'cache')->data;
    $this->info = &$info_all[$this->cacheKey];
    $this->info = new stdClass();
    $this->info->bin    = 'cache';
    $this->info->expire = CACHE_PERMANENT;

    // Modules can create custom cache bins so allow alters by them.
    $cache_key  = $this->cacheKey;
    $info_alter = array('bin' => &$this->info->bin, 'expire' => &$this->info->expire);
    drupal_alter('hex_cache_info', $cache_key, $info_alter);
    cache_set($iid, $info_all, 'cache');

    return $this;
  }

  /**
   * Rebuilds the cache data.
   */
  protected function rebuildData($cid) {
    $new_data = NULL;
    $existing = cache_get($cid, $this->info->bin);

    if ($this->cacheKey === 'plugins') {
      $components = hex()->listType('theme');
    }
    else {
      $components = array();
      foreach (hex()->listType('theme') as $theme) {
        $components += hex()->listType('plugin', $theme) + array($theme => $theme);
      }
    }
    foreach ($components as $component) {
      $build_func = "{$component}_{$this->cacheKey}_build";
      if (hex_function_exists($build_func)) {
        $build_func($new_data, $existing, $build_target = hex()->theme());
      }
    }
    cache_set($cid, $new_data, $this->info->bin, $this->info->expire);

    return $this;
  }

}
