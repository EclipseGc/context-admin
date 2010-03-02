<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Create User'),
  'description' => t('Allows the creation of a user.  This circumvents the typical drupal access controls on user creation.'),
  'context' => 'global',
);


function context_admin_user_create_menu_content_form(&$form, &$form_state, $cache = NULL) {
  if (!is_null($form_state['handler_id'])) {
    $default = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_user_roles'];
    $default_alter = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles_alter'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles_alter'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_user_roles_alter'];
    $default_help = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_registration_help'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_registration_help'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_user_registration_help'];
    $default_notify = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_notify'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_notify'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_user_notify'];
  }
  else {
    $default = $form_state['page']->new_handler->conf['context_admin_user_roles'];
    $default_alter = $form_state['page']->new_handler->conf['context_admin_user_roles_alter'];
    $default_help = $form_state['page']->new_handler->conf['context_admin_user_registration_help'];
    $default_notify = $form_state['page']->new_handler->conf['context_admin_user_notify'];
  }
  $roles = user_roles(TRUE);
  $options = array();
  foreach ($roles as $key => $role) {
    $options[$key] = $role;
  }
  $form['context_admin_user_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select the a default role for users created from this administration, if necessary.'),
    '#options' => $options,
    '#default_value' => is_array($default) ? $default : array($default),
  );
  $form['context_admin_user_roles_alter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Alter Roles'),
    '#description' => t('Allow users of this form to set any role.'),
    '#default_value' => $default_alter,
  );
  /*$form['context_admin_user_registration_help'] = array(
    '#type' => 'checkbox',
    '#title' => t('Registration Help'),
    '#description' => t('Includes the User Registration Help for this instance of user registration.'),
    '#default_value' => $default_help,
  );*/
  $form['context_admin_user_notify'] = array(
    '#type' => 'checkbox',
    '#title' => t('User Notification'),
    '#description' => t('Include the User Notification option.'),
    '#default_value' => $default_notify,
  );
  return $form;
}

function context_admin_user_create_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if (!is_null($form_state['handler_id'])) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles'] = $form_state['values']['context_admin_user_roles'];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_roles_alter'] = $form_state['values']['context_admin_user_roles_alter'];
    //$form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_registration_help'] = $form_state['values']['context_admin_user_registration_help'];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_user_notify'] = $form_state['values']['context_admin_user_notify'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_options_items'] = $form_state['values']['context_admin_options_items'];
    $form_state['page']->new_handler->conf['context_admin_options_items_alter'] = $form_state['values']['context_admin_options_items_alter'];
    //$form_state['page']->new_handler->conf['context_admin_user_registration_help'] = $form_state['values']['context_admin_user_registration_help'];
    $form_state['page']->new_handler->conf['context_admin_user_notify'] = $form_state['values']['context_admin_user_notify'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_user_create_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
  return drupal_get_form('context_admin_user_create_menu_render_form', $handler, $base_contexts, $args, $test);
}

function context_admin_user_create_menu_render_form($form_state, $handler, $base_contexts, $args, $test = TRUE) {
  $form = array();

  // Display the registration form.
  if ($handler->conf['context_admin_user_registration_help']) {
    $form['user_registration_help'] = array(
      '#value' => filter_xss_admin(variable_get('user_registration_help', '')),
      // Ensure that user registration help appears above profile fields.
      '#weight' => -20,
    );
  }

  // Merge in the default user edit fields.
  $form = array_merge($form, user_edit_form($form_state, NULL, NULL, TRUE));
  if ($handler->conf['context_admin_user_notify']) {
    $form['account']['notify'] = array(
     '#type' => 'checkbox',
     '#title' => t('Notify user of new account')
    );
  }
  // Redirect back to page which initiated the create request;
  $form['destination'] = array('#type' => 'hidden', '#value' => $_GET['q']);


  // Create a dummy variable for pass-by-reference parameters.
  $null = NULL;
  $extra = _user_forms($null, NULL, NULL, 'register');

  // Remove form_group around default fields if there are no other groups.
  if (!$extra) {
    foreach (array('name', 'mail', 'pass', 'status', 'roles', 'notify') as $key) {
      if (isset($form['account'][$key])) {
        $form[$key] = $form['account'][$key];
      }
    }
    unset($form['account']);
  }
  else {
    $form = array_merge($form, $extra);
  }

  if (variable_get('configurable_timezones', 1)) {
    // Override field ID, so we only change timezone on user registration,
    // and never touch it on user edit pages.
    $form['timezone'] = array(
      '#type' => 'hidden',
      '#default_value' => variable_get('date_default_timezone', NULL),
      '#id' => 'edit-user-register-timezone',
    );

    // Add the JavaScript callback to automatically set the timezone.
    drupal_add_js('
// Global Killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    Drupal.setDefaultTimezone();
  });
}', 'inline');
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Create new account'), '#weight' => 30);
  $form['#validate'][] = 'user_register_validate';
  $form['#submit'][] = 'user_register_submit';

  return $form;
}

function context_admin_user_create_menu_form_alter(&$form, $form_state, $form_id) {
  $page = page_manager_get_current_page();
  if ($form_id == 'context_admin_user_create_menu_render_form' && $page) {
    $form['roles']['#value'] = $page['handler']->conf['context_admin_user_roles'];
    $form['roles']['#value'][2] = TRUE;
    if (!$page['handler']->conf['context_admin_user_roles_alter']) {
      $form['roles']['#disabled'] = TRUE;
    }
  }
}