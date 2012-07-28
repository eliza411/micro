<?php

/**
 * This file can host variables for "theme functions" and "templates".
 *
 * Variables can be accessed multiple ways. The standard $variables or $vars
 * or any of the keys within the variables table.
 * 
 * The following will all point to the same variable.
 *  - $variables['element']
 *  - $vars['element']
 *  - $element
 * 
 * When working with the variable directly, i.e., not through the $variables
 * or $vars keys, any new variables need to be defined before it can be
 * accessed from a template or theme function.
 * 
 * It is good practice to define new variables at the top of the file so it is
 * easily visible on what the file introduced.
 */

// New variables referencing $vars. NULL at this point.
// This is important when working with variables directly and it need to be
// exposed in the template or theme function.
$my_new_variable_a = &$vars['my_new_variable_a'];
$my_new_variable_b = &$vars['my_new_variable_b'];
$my_new_variable_c = &$vars['my_new_variable_c'];
// Now with new value.
$my_new_variable_a = 'new-value';

// Existing variable with a new value.
$element = 'element-value';
// The new value will be reflected in $variables and $vars.
print $vars['element'];

// Usual way of working with variables.
$vars['other_element'] = 'other-value';
// $vars and $variables point to the same data.
print $variables['other_element'];
