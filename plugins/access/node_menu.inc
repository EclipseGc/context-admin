<?php
//$Id: node_menu.inc,v 1.2 2010/03/11 04:42:45 eclipsegc Exp $

/**
 * @file
 * Plugin to provide access control based upon existence in a particular menu.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Node: In Menu"),
  'description' => t('Control access based on whether the current node exists in a particular menu.'),
  'callback' => 'context_access_node_menu_access_check',
  'default' => array('menu' => array()),
  'settings form' => 'context_access_node_menu_access_settings',
  'settings form submit' => 'context_access_node_menu_access_settings_submit',
  'summary' => 'context_access_node_menu_access_summary',
  'required context' => new ctools_context_required(t('Node'), 'node'),
  'restrictions' => 'context_access_node_menu_access_restrictions',
);

function context_access_node_menu_get_menus() {
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
function context_access_node_menu_access_settings(&$form, &$form_state, $conf) {
  $options = context_access_node_menu_get_menus();

  $form['settings']['menu'] = array(
    '#title' => t('Menu'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Only the checked menus will be valid.'),
    '#default_value' => $conf['menu'],
  );
}

/**
 * Compress the node_types allowed to the minimum.
 */
function context_access_node_menu_access_settings_submit(&$form, &$form_state) {

}

/**
 * Check for access.
 */
function context_access_node_menu_access_check($conf, $context) {
  // Per the example in node type in core ctools
  if (empty($context) || empty($context->data) || empty($context->data->nid)) {
    return FALSE;
  }

  $results = db_query("SELECT menu_name FROM {menu_links} WHERE link_path = 'node/%d' AND module = 'menu'", $context->data->nid);
  while ($result = db_fetch_object($results)) {
    if ($result->menu_name == $conf['menu']) {
      return TRUE;
    }
  }

  if (empty($conf['menu'])) {
    return FALSE;
  }

  return FALSE;
}

/**
 * Inform the UI that we've eliminated a bunch of possibilities for this
 * context.
 */
function context_access_node_menu_access_restrictions($conf, &$context) {
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
function context_access_node_menu_access_summary($conf, $context) {
  if (!isset($conf['menu'])) {
    return t('No menu context has been selected for @identifier', array('@identifier' => $context->identifier));
  }

  return t('@identifier is in the "@menu" menu', array('@menu' => $conf['menu'], '@identifier' => $context->identifier));
}
