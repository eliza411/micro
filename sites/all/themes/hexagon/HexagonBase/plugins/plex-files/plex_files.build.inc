<?php

/**
 * implements hook_hook_build()
 */
function plex_files_plex_files_build(&$files, $existing, $build_target) {
  $scanner = new PlexFileScanner();
  $files   = $scanner->getArrayCopy();
}

/**
 * Scans for files.
 */
class PlexFileScanner extends HexFileScanner {

  /**
   * Initialize and build.
   */
  public function __construct($theme = NULL) {
    if (!isset($theme)) $theme = hex()->theme();
    if (hex()->validate($theme, __FUNCTION__, 'theme')) {
      $this->offsetEnd($theme);
      $scanned_files = array();
      foreach (hex()->listType('theme') as $theme) {
        // Scanning can only start from the current theme level. This prevents
        // sub-theme defined extensions and patterns from scanning it's parents.
        $this->offsetStart($theme);
        foreach ($this->getPatterns($theme) as $scope => $namespaces) {
          foreach ($namespaces as $namespace => $patterns) {
            foreach ($patterns as $mask) {
              // Empty out the ArrayObject and scan for new files.
              $this->exchangeArray(array());
              foreach ($this->scan($mask, $scope) as $file) {
                $virtual_path = $namespace ? "$namespace/{$file->filename}" : $file->filename;
                $scanned_files[$virtual_path][$file->uri] = $file;
              }
            }
          }
        }
      }
      $this->exchangeArray($scanned_files);
    }
  }

  /**
   * Returns patterns defined in info files and hook_plex_file_patterns().
   *
   * File extensions are merged into patterns.
   */
  public function getPatterns($theme) {
    // Collect all info defined code categories before scanning.
    $patterns = $extensions = array();
    foreach (hex()->listType('plugin', $theme) + array($theme) as $component) {
      // .info/.pinfo defined extensions.
      foreach ((array) hex()->info('plex file extensions', $component) as $scope => $namespaces) {
        foreach ($namespaces as $namespace => $extension) {
          if (is_int($namespace)) $namespace = '';
          $extensions[$scope][$namespace][] = preg_quote($extension, '/');
        }
      }
      // .info/.pinfo defined patterns.
      foreach ((array) hex()->info('plex file patterns', $component) as $scope => $namespaces) {
        foreach ($namespaces as $namespace => $pattern) {
          if (is_int($namespace)) $namespace = '';
          $patterns[$scope][$namespace][] = $pattern;
        }
      }
      // hook_plex_file_patterns().
      if (hex_function_exists($component . '_plex_file_patterns')) {
        foreach (call_user_func($component . '_plex_file_patterns') as $scope => $namespaces) {
          foreach ($namespaces as $namespace => $pattern) {
            if (is_int($namespace)) $namespace = '';
            $patterns[$scope][$namespace][] = $pattern;
          }
        }
      }
    }
    // Merge extensions into patterns.
    foreach ($extensions as $scope => $namespaces) {
      foreach ($namespaces as $namespace => $items) {
        $patterns[$scope][$namespace][] = "/^[[:alnum:]][[:alnum:]._-]+(" . implode('|', $items) . ")$/";
      }
    }
    return $patterns;
  }

}