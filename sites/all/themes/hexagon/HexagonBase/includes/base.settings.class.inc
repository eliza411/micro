<?php

/*
  TODO : all functionality related to settings should be accessible from here.
*/

class HexSettings {
  const FIRST_PARENT = '#first_parent';
  const LAST_CHILD   = '#last_child';

  protected $source;
  protected $build;

  public function __construct() {
    $this->source  = self::LAST_CHILD;
  }
  public function firstParent() {
    $this->source = self::FIRST_PARENT;
    return $this;
  }
  public function lastChild() {
    $this->source = self::LAST_CHILD;
    return $this;
  }
  public function activeTheme() {
    $this->source = HEX_ACTIVE_THEME;
    return $this;
  }
  public function currentTheme() {
    $this->source = hex()->theme();
    return $this;
  }
  public function get($setting) {
    $settings = hex_cache('settings')->data;
    print var_dump((array)$settings);
    return $value = &$settings[$setting][$this->source]->value;
  }
  public function getParents($setting) {
    $settings = hex_cache('settings')->data;
    return $parents = &$settings[$setting][$this->source]->dependencies;
  }
}
