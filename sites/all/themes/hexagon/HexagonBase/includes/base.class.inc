<?php

/**
 * Hex components interface.
 *
 * @see HexBaseInterface::attachComponent()
 */
interface HexComponentsInterface {
  public function type();
  public function listAll();
  public function getPath($name);
  public function getInfo($name);
}

/**
 * Hex base class.
 */
class HexBase {
  protected $context;
  protected $setContext;
  protected $components;
  protected $static;

  /**
   * Initialize.
   */
  public function __construct() {
    $this->context        = new stdClass();
    $this->context->theme = HEX_ACTIVE_THEME;
    $this->components = new SplObjectStorage();
    $this->static     = hex_static(__CLASS__);
  }

  /**
   * Returns the current theme. This is contextual.
   */
  public function theme() {
    return $this->context->theme;
  }

  /**
   * Switches the theming context.
   */
  public function setTheme() {
    if (!isset($this->setContext)) {
      // This setter will reset the static for this instance.
      $this->setContext = new HexThemeContextSetter($this->context);
    }
    return $this->setContext;
  }

  /**
   * Attaches component.
   */
  public function attach(HexComponentsInterface $component) {
    // Eache component type must be unique.
    if (!$this->typeExists($component->type())) {
      $this->components->attach($component);
      hex_static_reset(__CLASS__);
    }
    return $this;
  }

  /**
   * Returns all attached types.
   */
  public function listTypes() {
    $types = array();
    foreach ($this->components as $component) $types[] = $component->type();
    return $types;
  }

  /**
   * Returns the type for the given $name.
   */
  public function type($name) {
    if (($type = &$this->static->data[__METHOD__][$name]) === NULL
    &&  $name && $this->validate($name, __METHOD__)) {
      $type = current(explode(':', array_search($name, $this->listAll())));
    }
    return $type;
  }

  /**
   * Returns a list of names for the given $type.
   */
  public function listType($type, $host = NULL) {
    if (($list = &$this->static->data[__METHOD__]["$type:$host"]) === NULL
    &&  $type && $this->validate($type, __METHOD__, 'type')
    &&  (!$host || $this->validate($host, __METHOD__))) {
      $list = array();
      foreach ($this->getComponent($type)->listAll() as $key => $name) {
        if (!$host || strpos($key, "$host:") === 0) {
          $list["$type:$key"] = $name;
        }
      }
    }
    return $list;
  }

  /**
   * Returns a list of all active components.
   */
  public function listAll() {
    if (($list = &$this->static->data[__METHOD__]) === NULL) {
      $list = array();
      foreach ($this->components as $component) {
        $list += $this->listType($component->type());
      }
    }
    return $list;
  }

  /**
   * Return the path for the given $name.
   */
  public function path($name) {
    if (($path = &$this->static->data[__METHOD__][$name]) === NULL
    &&  $name && $this->validate($name, __METHOD__)) {
      $path = $this->getComponent($this->type($name))->getPath($name);
    }
    return $path;
  }

  /**
   * Return info data for the given $info_key within $name.
   */
  public function info($info_key, $name) {
    if (($info = &$this->static->data[__METHOD__][$name]) === NULL
    &&  $name && $this->validate($name, __METHOD__)) {
      $info = $this->getComponent($this->type($name))->getInfo($name);
    }
    return $value = &$info[$info_key];
  }

  /**
   * Checks if $type exists.
   */
  public function typeExists($type) {
    return $this->getComponent($type) !== NULL;
  }

  /**
   * Checks if $name exists.
   *
   * @param $name
   *  The name of the component. It can be a simple string or use a strict
   *  form, e.g., 'type:host:name', or 'type:name' depending on if the component
   *  is hosted in another component or not.
   */
  public function exists($name, $type = NULL) {
    $list = array();
    if (!isset($type)) {
      $list = $this->listAll();
    }
    elseif ($this->typeExists($type)) {
      $list = $this->listType($type);
    }
    return strpos($name, ':') !== FALSE ? isset($list[$name]) : in_array($name, $list);
  }

  /**
   * Validates that an item exists.
   */
  public function validate($item, $called, $type = NULL) {
    if ($type === 'type' && $this->typeExists($item) || $this->exists($item, $type)) {
      return TRUE;
    }
    else {
      trigger_error(t('%var is not available in %called.',
                      array('%var' => (isset($type) ? "$type $item" : $item), '%called' => $called)),
                    $type === 'type' ? E_USER_WARNING : E_USER_NOTICE);
      return FALSE;
    }
  }

  /**
   * Returns the attached component for a given $type.
   */
  protected function getComponent($type) {
    foreach ($this->components as $component) {
      if ($component->type() === $type) return $component;
    }
  }

}

/**
 * Hex component for themes.
 */
class HexThemes implements HexComponentsInterface {

  /**
   * Returns the type.
   */
  public function type() {
    return 'theme';
  }

  /**
   * Returns a list of active themes sorted base theme first.
   */
  public function listAll() {
    $list   = array();
    $themes = list_themes();
    $theme  = hex()->theme();
    while (isset($theme)) {
      $list[$theme] = $theme;
      $theme = isset($themes[$theme]->base_theme) ? $themes[$theme]->base_theme : NULL;
    }
    return array_reverse($list);
  }

  /**
   * Return the path for the given $name.
   */
  public function getPath($name) {
    return drupal_get_path('theme', $name);
  }

  /**
   * Return info data for the given $name.
   */
  public function getInfo($name) {
    $themes = list_themes();
    return $themes[$name]->info;
  }

}

/**
 * Hex component for plug-ins.
 */
class HexPlugins implements HexComponentsInterface {

  /**
   * Returns the type.
   */
  public function type() {
    return 'plugin';
  }

  /**
   * Returns a list of active plug-ins.
   */
  public function listAll() {
    $list = array();
    // If locked, reset until it's unlocked.
    if (hex_cache('plugins')->locked) hex_static_reset('HexBase');
    foreach ((array) hex_cache('plugins')->data as $name => $plugin) {
      $list["{$plugin->host}:$name"] = $name;
    }
    return $list;
  }

  /**
   * Return the path for the given $name.
   */
  public function getPath($name) {
    return hex_cache('plugins')->data[$name]->dir;
  }

  /**
   * Return info data for the given $name.
   */
  public function getInfo($name) {
    return hex_cache('plugins')->data[$name]->info;
  }

}

/**
 * Returns a cache object.
 *
 * @ingroup hex_cache
 */
class HexCache {
  // Tracks locked states accross instances.
  static protected $locks;

  protected $infoId;
  protected $cacheKey;
  protected $dataId;
  protected $lock;
  protected $build;

  public $buildTarget;
  public $cid;
  public $locked;
  public $refreshed;
  public $bin;
  public $expire;
  public $created;
  public $data;

  /**
   * Initialize.
   */
  public function __construct($cache_key) {
    $this->cacheKey = $cache_key;
    $this->fetch();
  }

  /**
   * Fetches the cached data.
   */
  public function fetch() {
    if ($cache = $this->getCache()) {
      $this->data = $cache->data;
    }
    elseif ($cache = $this->refresh()->getCache()) {
      $this->data = $cache->data;
    }
    elseif (!$this->lock) {
      $this->data = NULL;
      trigger_error(t('Cache key %arg is not available.', array('%arg' => $this->cacheKey)), E_USER_ERROR);
    }
    return $this;
  }

  /**
   * refreshes the cache.
   *
   * When the cache is refreshed, getCache() must be called to retrieve it.
   */
  public function refresh() {
    if (!isset($this->build)) {
      $this->build = new HexCacheBuild($this->cacheKey);
    }
    // A refresh allowed only when the cache is not locked.
    if (!$this->lock) {
      $this->lock = TRUE;
      $this->setContext()->build->rebuild($this->infoId, $this->dataId);
      $this->lock = FALSE;
    }
    return $this;
  }

  /**
   * Returns the cached data.
   */
  protected function getCache() {
    $this->setContext();
    $cache = FALSE;
    if ($info = &cache_get($this->infoId, 'cache')->data[$this->cacheKey]
    &&  $cache = cache_get($this->dataId, $info->bin)) {
      $this->bin       = $info->bin;
      $this->expire    = $info->expire;
      $this->created   = $cache->created;
    }
    return $cache;
  }

  /**
   * Sets the context for the cache.
   */
  protected function setContext() {
    $this->infoId = 'hex_cache_info:' . hex()->theme();
    $this->dataId = 'hex_cache:' . hex()->theme() . ':' . $this->cacheKey;
    $this->lock   = &self::$locks[$this->dataId];
    if (!isset($this->lock)) $this->lock = FALSE;
    $this->buildTarget = hex()->theme();
    $this->cid         = $this->dataId;
    $this->locked      = $this->lock;
    $this->refreshed   = !empty($this->build);
    $this->bin         = NULL;
    $this->expire      = NULL;
    $this->created     = NULL;
    return $this;
  }

}
