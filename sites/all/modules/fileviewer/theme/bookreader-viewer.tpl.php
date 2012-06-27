<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">

<head>
  <title><?php print t('File viewer'); ?></title>
  <?php
      if (module_exists('admin_menu')) {
        admin_menu_suppress();
      }
      $filepath = fileviewer_path($variables['file']->fid);
      $pdf_uri = file_create_url('public://' . variable_get('fileviewer_path', 'fileviewer') . '/' . $file->fid);
      drupal_add_js(array('fileviewer' => array('pdfFolder' => $pdf_uri)), 'setting');
      $images = file_scan_directory($filepath, '/.*\.png/');
      sort($images);
//      $images = file_scan_directory(drupal_realpath(file_build_uri(variable_get('fileviewer_path', 'fileviewer') . '/' . $file->fid)), '/.*\.png/');
      if (variable_get('fileviewer_pdf_iabookreader_search_inside', TRUE) == TRUE) {
        drupal_add_js(array('fileviewer' => array('searchInside' => true)), 'setting');
      }
      if (user_access('download file from fileviewer')) {
        drupal_add_js(array('fileviewer' => array('filename' => $file->filename)), 'setting');
        drupal_add_js(array('fileviewer' => array('filepath' => file_create_url($file->uri))), 'setting');
      }
      else {
        drupal_add_js(array('fileviewer' => array('filepath' => '', 'filename' => $file->filename)), 'setting');
      }
      $cover = reset($images);
      $image_size = getimagesize($cover->uri);
      drupal_add_js(array('fileviewer' => array('pageNumber' => count($images), 'pageWidth' => $image_size[0], 'pageHeight' => $image_size[1])), 'setting');
      global $base_url;
      $libraries = libraries_get_libraries();
drupal_add_js(array('fileviewer' => array('bookId' => $file->fid, 'server' => $_SERVER['SERVER_NAME'] . base_path() . 'fileviewer', 'imagesBaseURL' => $base_url . '/' . $libraries['BookReader'] . '/images/')), 'setting');
      
      drupal_add_css($libraries['BookReader'] . '/BookReader.css');
      drupal_add_css($libraries['BookReader'] . '/BookReaderEmbed.css');

      drupal_add_js($libraries['BookReader'] . '/jquery-ui-1.8.5.custom.min.js');
      drupal_add_js($libraries['BookReader'] . '/dragscrollable.js');
      drupal_add_js($libraries['BookReader'] . '/jquery.colorbox-min.js');
      drupal_add_js($libraries['BookReader'] . '/jquery.bt.min.js');
      drupal_add_js($libraries['BookReader'] . '/BookReader.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/js/bookreader.js', array('scope' => 'footer'));
      drupal_add_css(drupal_get_path('module', 'fileviewer') . '/fileviewer.css');
  ?>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php $vars['scripts'] = drupal_get_js('header'); ?>
  <?php function_exists('jquery_update_preprocess_page') && jquery_update_preprocess_page($vars); ?>
  <?php print $vars['scripts']; ?>
  <style media="all" type="text/css">/*Quick-override*/</style>
</head>

<body style="background-color: ##939598;">
  <div id="filevieer-messages"><?php print theme('status_messages'); ?></div>
  <div  class="fileviwer">
    <div id="BookReader">
    </div>
  </div>
  <?php print drupal_get_js('footer'); ?>
</body>

</html>
