<?php

/**
 * ArrayObject for generating a namespaced array.
 */
class HexNameSpaces extends ArrayObject {
  protected $delimiter;
  protected $expanded;
  protected $callback;

  /**
   * Initialize.
   *
   * @param $array
   *  Input array.
   * @param $delimiter
   *  Character used to delimit name spaces.
   * @param $iterator_class
   *  Specify the class that will be used for iteration of the ArrayObject
   *  object. ArrayIterator is the default class used.
   */
  public function __construct ($input = array(), $delimiter = '.', $iterator_class = 'ArrayIterator') {
    parent::__construct(array(), ArrayObject::STD_PROP_LIST, $iterator_class);
    $this->delimiter = $delimiter;
    $this->expanded  = array();
    $this->add($input);
  }

  /**
   * Setting flags not allowed. Defaults to ArrayObject::STD_PROP_LIST.
   */
  public function setFlags($flag) {
    trigger_error(t('Setting flags it not permitted.'), E_USER_NOTICE);
    return $this;
  }

  /**
   * Sets a callback for each value on add().
   */
  public function setValueCallback($callback) {
    if (is_callable($callback)) {
      $this->callback = $callback;
    }
    else {
      trigger_error(t('The callback is not callable.'), E_USER_WARNING);
    }
    return $this;
  }

  /**
   * Unsets the callback for each value on add().
   */
  public function unsetValueCallback() {
    $this->callback = NULL;
    return $this;
  }

  /**
   * Returns an expanded copy.
   */
  public function getExpandedCopy() {
    return $this->expand()->expanded;
  }

  /**
   * Converts and adds multi-dimensional arrays as a flat name spaced array.
   *
   * The following assumes the default '.' delimiter.
   *
   * From this:
   *  array(
   *    'foo' => array(
   *      'bar' => array(
   *        'baz' => TRUE,
   *      ),
   *    ),
   *  );
   *
   * To this:
   *  array(
   *    'foo.bar.baz' = TRUE,
   *  );
   *
   * @param $array
   *  The array or ArrayObject to add. Can be any level deep.
   */
  public function add($array) {
    if ($array instanceof ArrayObject) {
      $array = $array->getArrayCopy();
    }
    if (is_array($array)) {
      $it = new RecursiveIteratorIterator(new RecursiveArrayIterator($array), RecursiveIteratorIterator::SELF_FIRST);
      foreach ($it as $key => $value) {
        $keys[$it->getDepth()] = $key;
        $keys = array_splice($keys, 0, $it->getDepth() + 1);
        if (!is_array($value)) {
          if ($this->callback) {
            $value = call_user_func($this->callback, $value);
          }
          $this->offsetSet(implode($this->delimiter, $keys), $value);
        }
      }
    }
    else {
      trigger_error(t('Only Arrays or ArrayObjects are accepted.'), E_USER_WARNING);
    }
    return $this;
  }

  /**
   * Expands a name spaced array into name spaced multi-dimensional array.
   *
   * The following assumes the default '.' delimiter.
   *
   * From this:
   *  array(
   *    'foo.bar.baz' = TRUE,
   *  );
   *
   * To this:
   *  array(
   *    'foo.bar.baz' = & TRUE,
   *    'foo.bar' = & array(
   *      'baz' => & TRUE,
   *    ),
   *    'foo' => array(
   *      'bar' => & array(
   *        'baz' => & TRUE,
   *      ),
   *    ),
   *  );
   *
   * Note that all the values in the expansion are referenced to one another.
   */
  protected function expand() {
    $array = $this->getArrayCopy();
    $split = preg_quote($this->delimiter);
    foreach (preg_grep("/.+$split.+/", array_keys($array)) as $named) {
      $parent = &$array;
      $keys   = explode($this->delimiter, $named);
      $last   = array_pop($keys);

      // Create nested arrays
      $rkeys = array();
      foreach ($keys as $key) {
        $key = hex_auto_type($key);
        if (!isset($parent[$key]) || !is_array($parent[$key])) {
          $parent[$key] = array();
        }
        // Build nested arrays, e.g.,
        // - foo
        //     bar
        //       baz
        $parent = &$parent[$key];
        // Slice out each sub-array keyed by namespace, e.g.,
        // - foo
        // - foo.bar
        // - foo.bar.baz
        $rkeys[] = $key;
        $array[implode($this->delimiter, $rkeys)] = &$parent;
      }

      // Insert actual value with the last key.
      $parent[hex_auto_type($last)] = &$array[$named];
    }
    $this->expanded = $array;
    return $this;
  }

}

/**
 * Tools for scanning file paths.
 */
class HexFileScanner extends ArrayObject {
  protected $options = array('nomask' => '/^(CVS|~.*)$/');
  protected $offsetStart;
  protected $offsetEnd;

  /**
   * Set options for file scanning.
   *
   * @param $key
   *   - 'nomask': The preg_match() regular expression of the files to ignore.
   *     Defaults to '/^(CVS|~.*)$/'. Files and directories beginning with a dot
   *     will always be ignored.
   *   - 'callback': The callback function to call for each match. There is no
   *     default callback.
   *   - 'recurse': When TRUE, the directory scan will recurse the entire tree
   *     starting at the provided directory. Defaults to TRUE.
   *   - 'key': The key to be used for the returned associative array of files.
   *     Possible values are 'uri', for the file's URI; 'filename', for the
   *     basename of the file; and 'name' for the name of the file without the
   *     extension. Defaults to 'uri'.
   *   - 'min_depth': Minimum depth of directories to return files from. Defaults
   *     to 0.
   * @param $value
   *   The value for the $key.
   */
  public function setOption($key, $value) {
    $this->options[$key] = $value;
    return $this;
  }

  /**
   * The offset to start the scan. This must be an running theme. The scanning
   * starts from the base theme to sub-themes along with their hosted plug-ins.
   *
   * @param $theme
   *  The theme to set for the offset.
   */
  public function offsetStart($theme) {
    if ($theme && hex()->validate($theme, __METHOD__, 'theme')) {
      $this->offsetStart = $theme;
    }
    return $this;
  }

  /**
   * The offset to end the scan. This must be an running theme. The scanning
   * starts from the base theme to sub-themes along with their hosted plug-ins.
   *
   * @param $theme
   *  The theme to set for the offset.
   */
  public function offsetEnd($theme) {
    if ($theme && hex()->validate($theme, __METHOD__, 'theme')) {
      $this->offsetEnd = $theme;
    }
    return $this;
  }

  /**
   * Scans for files.
   *
   * @see file_scan_directory()
   *  The function does the actual scanning.
   *
   * @param $mask
   *  The preg_match() regular expression of the files to find.
   * @param $scope
   *  Can be 'theme', 'plugin' or 'all' (default). It can also be a specific
   *  plugin or theme key, whichever "owns" the file.
   */
  public function scan($mask, $scope = 'all') {
    foreach ($this->scanningContexts($scope) as $context) {
      $scanned_files = file_scan_directory($context->dir, $mask, $this->options);
      $scanned_keys  = array_keys($scanned_files);
      if ($context->ignore && $scanned_keys) {
        // Negated $context->ignore.
        $scanned_keys = preg_grep("{^(?!{$context->ignore}).*}", $scanned_keys);
      }
      foreach ($scanned_keys as $key) {
        $file = $scanned_files[$key];
        $file->dir   = dirname($file->uri);
        $file->host  = $context->host;
        $file->hosts = $context->hosts;
        $this->offsetSet($key, $file);
      }
    }
    return $this;
  }

  /**
   * Provides scanning contexts for the given $scope.
   */
  protected function scanningContexts($scope) {
    $contexts = &hex_static(__METHOD__)->data["$scope:{$this->offsetStart}-{$this->offsetEnd}"];
    if (!isset($contexts)) {
      $contexts = array();

      $start = !$this->offsetStart;
      foreach (hex()->listType('theme') as $theme) {
        if ($theme === $this->offsetStart) $start = TRUE;
        foreach (hex()->listType('plugin', $theme) as $plugin) {
          if (in_array($scope, array($plugin, 'plugin', 'all'))) {
            $context         = new stdClass();
            $context->host   = $plugin;
            $context->hosts  = array($plugin, $theme);
            $context->dir    = hex()->path($plugin);
            $context->ignore = implode('|', array_map('preg_quote', preg_grep('/^' . preg_quote(hex()->path($plugin), '/') . '.+/', $this->getIgnored())));
            $contexts[]      = $context;
          }
        }
        if (in_array($scope, array($theme, 'theme', 'all'))) {
          $context         = new stdClass();
          $context->host   = $theme;
          $context->hosts  = array($theme);
          $context->dir    = hex()->path($theme);
          $context->ignore = implode('|', array_map('preg_quote', preg_grep('/^' . preg_quote(hex()->path($theme), '/') . '.+/', $this->getIgnored())));
          $contexts[]      = $context;
        }
        if ($theme === $this->offsetEnd) break;
      }
    }
    return $contexts;
  }

  /**
   * Returns a list of paths to ignore.
   */
  protected function getIgnored() {
    // 'ignore' holds the paths to *all* themes and plug-ins even if they are 
    // not active. Duplicate and overridden paths for plug-ins are included.
    $ignore = array();
    foreach (list_themes() as $theme) {
      $ignore[] = dirname($theme->filename);
    }
    $mask = '/^' . DRUPAL_PHP_FUNCTION_PATTERN . '\.pinfo$/';
    $options = array('nomask' => '/^(CVS|~.*)$/');
    foreach ($ignore as $ignored) {
      foreach (file_scan_directory($ignored, $mask, $options) as $file) {
        $ignore[] = dirname($file->uri);
      }
    }
    return $ignore;
  }

}

/**
 * Tools for scanning file contents.
 */
class HexContentScanner extends HexFileScanner {

  /**
   * Scans files and their contents.
   *
   * @param $mask
   *  The preg_match() regular expression of the files to find.
   * @param $scope
   *  Can be 'theme', 'plugin' or 'all' (default). It can also be a specific
   *  plugin or theme key, whichever "owns" the file.
   */
  public function scan($mask, $scope = 'all') {
    // Exchange existing contents with an empty array before the file scan.
    $files_contents = $this->exchangeArray(array());
    parent::scan($mask, $scope);
    // Restore contents and get scanned files.
    $files = $this->exchangeArray($files_contents);
    foreach ($files as $key => $file) {
      $file->contents = $this->getContents($file->uri);
      $this->offsetSet($key, $file);
    }
    return $this;
  }

  /**
   * Returns all the functions, classes and interfaces within a file.
   *
   * @param $file
   *  The full path to the file.
   *
   * @return
   *  An object with the following properties.
   *  - functions
   *  - classes
   *  - interfaces
   */
  public function getContents($file) {
    $contents = &$this->contents[$file];
    if (!isset($contents)) {
      $types = array(
        T_FUNCTION  => 'function',
        T_CLASS     => 'class',
        T_INTERFACE => 'interface',
      );
      $contents = array();
      foreach ($types as $type) {
        $contents[$type] = array();
      }
      // T_FUNCTION doesn't differentiate between functions and class methods.
      // This doesn't ignore it if the methods doesn't define 'private',
      // 'public' or 'protected'.
      $ignored = array(
        T_PRIVATE   => T_FUNCTION,
        T_PUBLIC    => T_FUNCTION,
        T_PROTECTED => T_FUNCTION,
      );

      foreach (token_get_all(file_get_contents($file)) as $token) {
        if (is_string($token)) continue;
        if ($skip = &$ignored[$token[0]]) {
          $skip_next = $skip;
        }
        elseif ($match = &$types[$token[0]] && (!isset($skip_next) || $skip_next !== $token[0])) {
          $match_type = $match;
          $skip_next  = NULL;
        }
        elseif (isset($match_type) && $token[0] === T_STRING) {
          $contents[$match_type][] = $token[1];
          $match_type = NULL;
        }
      }
    }
    return $contents;
  }

}
