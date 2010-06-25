<?php
//$Id$

/**
 * @file
 * Plugin to provide access control based upon existence in a particular menu.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Node: Menu Item Depth"),
  'description' => t('Control access based upon the depth within a particular menu of this node.'),
  'callback' => 'context_access_node_menu_depth_access_check',
  'default' => array('menu' => array()),
  'settings form' => 'context_access_node_menu_depth_access_settings',
  'settings form submit' => 'context_access_node_menu_depth_access_settings_submit',
  'summary' => 'context_access_node_menu_depth_access_summary',
  'required context' => new ctools_context_required(t('Node'), 'node'),
  'restrictions' => 'context_access_node_menu_depth_access_restrictions',
);

function context_access_node_menu_depth_get_menus() {
  $system_menus = menu_list_system_menus();
  $custom_menus = menu_get_menus();
  foreach ($system_menus as $name) {
    $options[$name] = ucfirst(str_replace('-', ' ', $system_menus));
  }
  foreach ($custom_menus as $name => $title) {
    $options[$name] = $title;
  }
  return $options;
}

/**
 * Settings form for the 'by node_type' access plugin
 */
function context_access_node_menu_depth_access_settings(&$form, &$form_state, $conf) {
  $options = context_access_node_menu_depth_get_menus();

  $form['settings']['menu'] = array(
    '#title' => t('Menu'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Only the selected menu will be valid.'),
    '#default_value' => $conf['menu'],
  );
  $form['settings']['menu_depth'] = array(
    '#title' => t('Depth'),
    '#type' => 'select',
    '#options' => array(
      'p1' => '1',
      'p2' => '2',
      'p3' => '3',
      'p4' => '4',
      'p5' => '5',
      'p6' => '6',
      'p7' => '7',
      'p8' => '8',
      'p9' => '9',
    ),
    '#description' => t('The maximum depth from which this menu item is accessible.'),
    '#default_value' => $conf['menu_depth'],
  );
}

/**
 * Compress the node_types allowed to the minimum.
 */
function context_access_node_menu_depth_access_settings_submit(&$form, &$form_state) {

}

/**
 * Check for access.
 */
function context_access_node_menu_depth_access_check($conf, $context) {
  // Per the example in node type in core ctools
  if (empty($context) || empty($context->data) || empty($context->data->nid)) {
    return FALSE;
  }

  $results = db_query("SELECT mlid, p1, p2, p3, p4, p5, p6, p7, p8, p9 FROM {menu_links} WHERE link_path = 'node/%d' AND module = 'menu' AND menu_name = '%s'", $context->data->nid, $conf['menu']);
  while ($result = db_fetch_object($results)) {
    if ($result->mlid != $result->{$conf['menu_depth']}) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Inform the UI that we've eliminated a bunch of possibilities for this
 * context.
 */
function context_access_node_menu_depth_access_restrictions($conf, &$context) {
  if (isset($context->restrictions['menu'])) {
    $context->restrictions['menu'] = array_unique(array_merge($context->restrictions['menu'], array_keys(array_filter($conf['menu']))));
  }
  else {
    $context->restrictions['menu'] = $conf['menu'];
  }
}

/**
 * Provide a summary description based upon the checked node_types.
 */
function context_access_node_menu_depth_access_summary($conf, $context) {
  if (!isset($conf['menu'])) {
    return t('No menu context has been selected for @identifier', array('@identifier' => $context->identifier));
  }

  return t('@identifier is in the "@menu" menu', array('@menu' => $conf['menu'], '@identifier' => $context->identifier));
}
