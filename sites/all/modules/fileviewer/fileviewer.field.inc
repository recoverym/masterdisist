<?php
/**
 * Implements hook_field_formatter_info().
 */
function fileviewer_field_formatter_info() {
  return array(
    'fileviewer_default' => array(
      'label' => t('Fileviwer'),
      'field types' => array('file'),
    ),
    'fileviewer_embed' => array(
      'label' => t('Embed fileviwer'),
      'field types' => array('file'),
    ),
    'fileviewer_preview' => array(
      'label' => t('Fileviewer preview'),
      'field types' => array('file'),
      'settings' => array('image_style' => '', 'image_link' => ''),
    ),
    'fileviewer_google_docs' => array(
      'label' => t('Google Docs Viewer'),
      'field types' => array('file'),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function fileviewer_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  if ($instance['display'][$view_mode]['type'] == 'fileviewer_preview') {
    return image_field_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
  }
  else {
    return FALSE;
  }
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function fileviewer_field_formatter_settings_summary($field, $instance, $view_mode) {
  if ($instance['display'][$view_mode]['type'] == 'fileviewer_preview') {
    return image_field_formatter_settings_summary($field, $instance, $view_mode);
  }
  else {
    return FALSE;
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function fileviewer_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    case 'fileviewer_default':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#theme' => 'fileviewer_formatter_default',
          '#file' => (object) $item,
        );
      }
      break;
    case 'fileviewer_embed':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#theme' => 'fileviewer_formatter_embed',
          '#file' => (object) $item,
        );
      }
      break;
    case 'fileviewer_preview':
      // Check if the formatter involves a link.
      if ($display['settings']['image_link'] == 'content') {
        $uri = entity_uri($entity_type, $entity);
      }
      elseif ($display['settings']['image_link'] == 'file') {
        $link_file = TRUE;
      }
      foreach ($items as $delta => $item) {
        if (isset($link_file)) {
          $uri = array(
            'path' => file_create_url('file/'. $item['fid']),
            'options' => array(),
          );
        }
        $element[$delta] = array(
          '#theme' => 'fileviewer_formatter_preview',
          '#item' => $item,
          '#image_style' => $display['settings']['image_style'],
          '#path' => isset($uri) ? $uri : '',
        );
      }
      break;
    case 'fileviewer_google_docs':
      foreach ($items as $delta => $item) {
        $element[$delta] = array('#markup' => empty($item['uri']) ? '' : l ('View '. $item['filename']. ' in Google Docs', 'http://docs.google.com/viewer?url=' . file_create_url($item['uri'])));
      }
      break;
  }
  return $element;
}

function theme_fileviewer_formatter_default($variables) {
  $output = '<span class="file">' . theme('file_icon', $variables) . $variables['file']->filename . '</span>'; 
  $output .= '<div class="fileviewer">';
  $output .= format_size($variables['file']->filesize);
  $output .= '　';
  $output .= '<span>';
  $output .= l(t('View'), 'file/' . $variables['file']->fid); 
  if (user_access('download file from fileviewer')){
    $output .= '　';
    $output .= l(t('Download'), file_create_url($variables['file']->uri));
  }
  $output .= '</span>';
  $output .= '</div>';
  return $output;
}

function theme_fileviewer_formatter_embed($variables) {
  drupal_add_css(drupal_get_path('module', 'fileviewer') . '/fileviewer.css');
  global $base_url;
  $content = '<div class="fileviewer">';
  $content .= '<iframe class="fileviewer" frameborder="no" scrolling="no" src="';
  $content .= $base_url . '/file/' . $variables['file']->fid . '?ui=embed';
  $content .= '"></iframe>';
  $content .= '</div>';
  return $content;
}

function theme_fileviewer_formatter_preview($variables) {
  $item = $variables['item'];
  global $base_url;
  $filepath = fileviewer_path($item['fid']);
  $images = file_scan_directory($filepath, '/.*\.png/');
  sort($images);
  if (count($images) != 0) {
    $preview = reset($images);
    $image_file = ('public://' . variable_get('fileviewer_path', 'fileviewer') . '/' . $item['fid'] . '/' . $preview->filename);
  }
  else {
    $image_file = 'public://cover.png';
  }

  $image = array(
    'path' => $image_file,
//    'alt' => $item['alt'],
  );
  if ($variables['image_style']) {
    $image['style_name'] = $variables['image_style'];
    $output = theme('image_style', $image);
  }
  else {
    $output = theme('image', $image);
  }

  if ($variables['path']) {
    $path = $variables['path']['path'];
    $options = $variables['path']['options'];
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }
  return $output;
}
