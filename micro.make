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

projects[] = date
projects[] = fullcalendar

projects[] = views
projects[] = views_bulk_operations
projects[] = registration
projects[] = entity
projects[simple_gmap][version] = "1.x-dev" 
projects[] = masquerade
projects[] = entityreference

; Theme
projects[] = adaptivetheme
projects[] = corolla

; Libraries
libraries[tinymce][download][type] = "get"
libraries[tinymce][download][url] = "https://github.com/downloads/tinymce/tinymce/tinymce_3.4.8.zip"
libraries[tinymce][destination] = "libraries"
libraries[tinymce][directory_name] = "tinymce"

libraries[fullcalendar][download][type] = "get"
libraries[fullcalendar][download][url] = "http://arshaw.com/fullcalendar/downloads/fullcalendar-1.5.3.zip"
libraries[fullcalendar][download][subtree] = "fullcalendar"

; Theme
projects
 
