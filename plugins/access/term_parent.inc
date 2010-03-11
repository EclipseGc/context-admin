<?php
//$Id$

/**
 * @file
 * Plugin to provide access control based upon a term's parent tid.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Term: Parent"),
  'description' => t('Control access based upon the parent of the current term.'),
  'callback' => 'context_admin_term_parent_access_check',
  'default' => array('menu' => array()),
  'settings form' => 'context_admin_term_parent_access_settings',
  'settings form submit' => 'context_admin_term_parent_access_settings_submit',
  'summary' => 'context_admin_term_parent_access_summary',
  'required context' => new ctools_context_required(t('Term'), array('term', 'terms')),
  'restrictions' => 'context_admin_term_parent_access_restrictions',
);

/**
 * Settings form for the 'by node_type' access plugin
 */
function context_admin_term_parent_access_settings(&$form, &$form_state, $conf) {
  $form['settings']['parent'] = array(
    '#title' => t('Term parent'),
    '#type' => 'textfield',
    '#description' => t('Enter a numeric value for the parent of the term you wish to utilize.  Multiple terms may be comma separated.'),
    '#default_value' => $conf['parent'],
  );
}

/**
 * Compress the node_types allowed to the minimum.
 */
function context_admin_term_parent_access_settings_submit(&$form, &$form_state) {

}

/**
 * Check for access.
 */
function context_admin_term_parent_access_check($conf, $context) {
  // Per the example in node type in core ctools
  if (empty($context) || empty($context->data) || empty($context->data->tid)) {
    return FALSE;
  }

  $acceptable_parents = explode(',', $conf['parent']);
  $results = db_query("SELECT parent FROM {term_hierarchy} WHERE tid = %d", $context->data->tid);
  while ($result = db_fetch_object($results)) {
    if (in_array($result->parent, $acceptable_parents)) {
      return TRUE;
    }
  }

  if (empty($conf['parent'])) {
    return FALSE;
  }

  return FALSE;
}

/**
 * Inform the UI that we've eliminated a bunch of possibilities for this
 * context.
 */
function context_admin_term_parent_access_restrictions($conf, &$context) {
  if (!is_array($conf['parent'])) {
    $conf['parent'] = array($conf['parent']);
  }
  if (isset($context->restrictions['parent'])) {
    $context->restrictions['parent'] = array_unique(array_merge($context->restrictions['parent'], array_keys(array_filter($conf['parent']))));
  }
  else {
    $context->restrictions['parent'] = array_keys(array_filter($conf['parent']));
  }
}

/**
 * Provide a summary description based upon the checked node_types.
 */
function context_admin_term_parent_access_summary($conf, $context) {
  if (!isset($conf['parent'])) {
    return t('No parent context has been selected for @identifier', array('@identifier' => $context->identifier));
  }

  return t('@identifier is a child of term "@parent"', array('@parent' => $conf['parent'], '@identifier' => $context->identifier));
}
