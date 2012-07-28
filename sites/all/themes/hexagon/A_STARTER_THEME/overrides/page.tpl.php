<div id="page" class="clearfix">

  <div id="site-header" class="clearfix">
  <?php if ($site_id || $site_slogan): ?>
    <div class="site-branding">
    <?php if ($site_id) : ?>
      <h2 class="site-id">
        <?php print $site_id ?>
        <?php if ($site_slogan) : ?><p class="site-slogan"><?php print $site_slogan ?></p><?php endif ?>
      </h2>
    <?php endif ?>
    </div>
  <?php endif ?>

  <?php if ($main_menu_links || $secondary_menu_links): ?>
    <div class="site-navigation">
      <?php print $main_menu_links ?>
      <?php print $secondary_menu_links ?>
    </div>
  <?php endif ?>

    <?php print render($page['header']) ?>
  </div>

  <div id="page-header">
    <?php print $messages ?>
    <?php print render($page['highlighted']) ?>
  </div>

  <div id="page-main" class="clearfix">
    <div class="region-content-wrapper">
    <?php if ($breadcrumb || $title_prefix || $title || $title_suffix || $page['help']) : ?>
      <div class="content-header">
        <?php print $breadcrumb ?>
        <?php print render($title_prefix) ?>
        <?php if ($title) : ?><h1 class="page-title title"><?php print $title ?></h1><?php endif ?>
        <?php print render($title_suffix) ?>
        <?php print render($tabs) ?>
        <?php print render($page['help']) ?>
        <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>
      </div>
    <?php endif ?>

      <div id="main-content" class="clearfix">
        <?php print render($page['content']) ?>
        <?php print $feed_icons ?>
      </div>
    </div>

    <?php print render($page['sidebar_first']) ?>
    <?php print render($page['sidebar_second']) ?>
  </div>

  <div id="site-footer">
    <?php print render($page['footer']) ?>
  </div>

</div>
