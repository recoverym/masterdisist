<?php
/**
 * Implements hook_field_formatter_info().
 */
function pdf_field_formatter_info() {
  return array(
    'pdf_default' => array(
      'label' => t('PDF default'),
      'field types' => array('file', 'pdf'),
    ),
    'pdf_thumbnail' => array(
      'label' => t('PDF first page'),
      'field types' => array('file', 'pdf'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function pdf_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  switch ($display['type']) {
    case 'pdf_default':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#theme' => 'pdf_formatter_default',
          '#file' => $item,
        );
      }
      break;
    case 'pdf_thumbnail':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#theme' => 'pdf_formatter_thumbnail',
          '#file' => $item,
        );
      }
      break;
  }
  return $element;
}

function theme_pdf_formatter_default($variables) {
  if (isset($variables['file'])) {
    $file_url = file_create_url($variables['file']['uri']);
  }
  //TODO http://drupal.org/node/1540668
  elseif (arg(0) == 'file' && is_numeric(arg(1))) {
    $file = file_load(arg(1));
    $file_url = file_create_url($file->uri);
  }
  //$content = '<iframe class="pdf" frameborder="no" width="99%" height="600px" src="';
  //$content .= $GLOBALS['base_url'] . '/' . libraries_get_path('pdf.js') . '/web/viewer.html?file=' . $file_url;
  
  $content = '<iframe class="pdf" frameborder="no" width="99%" height="600px" src="';
  $content .= $GLOBALS['base_url'] . '/' . libraries_get_path('pdf.js') . '/web/viewer-embed.html?file=' . $file_url;
  
  $content .= '"></iframe>';
  
  return $content;
}

function theme_pdf_formatter_thumbnail($variables) {
  if (isset($variables['file'])) {
    $file_url = file_create_url($variables['file']['uri']);
  }
  //TODO http://drupal.org/node/1540668
  elseif (arg(0) == 'file' && is_numeric(arg(1))) {
    $file = file_load(arg(1));
    $file_url = file_create_url($file->uri);
  }
  drupal_add_js($GLOBALS['base_url'] . '/' . libraries_get_path('pdf.js') . '/build/pdf.js');

  $worker_loader = url('pdf/worker_loader.js');
  $js = "PDFJS.workerSrc = '$worker_loader';";
  $js .= "'use strict';
  PDFJS.getDocument('$file_url').then(function(pdf) {
    pdf.getPage(1).then(function(page) {
      var scale = 1.5;
      var viewport = page.getViewport(scale);
      var canvas = document.getElementById('the-canvas');
      var context = canvas.getContext('2d');
      canvas.height = viewport.height;
      canvas.width = viewport.width;
      var renderContext = {
        canvasContext: context,
        viewport: viewport
      };
      page.render(renderContext);
    });
  });
  ";

  drupal_add_js($js, array('type' => 'inline'));
  $output = '<canvas id="the-canvas" style="width:99%; border:1px solid black;"/>';
  return $output;
}

