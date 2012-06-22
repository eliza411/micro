core = "7.14"
api = 2 
projects[] = drupal

; Contrib
projects[] = token
projects[] = pathauto
projects[] = ctools
projects[] = environment_indicator
projects[] = features
projects[] = context
projects[] = strongarm

projects[] = views
projects[] = views_bulk_operations
projects[] = registration
projects[simple_gmap][version] = "1.x-dev" 
projects[] = masquerade


; Libraries
libraries[tinymce][download][type] = "get"
libraries[tinymce][download][url] = "https://github.com/downloads/tinymce/tinymce/tinymce_3.4.8.zip"
libraries[tinymce][destination] = "libraries"
libraries[tinymce][directory_name] = "tinymce"

; Theme
projects
 
