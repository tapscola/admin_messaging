<?php
// $Id$

 
/**
 * Form constructor function with the fields interface for autocomplete users
 */
function admin_messaging_multiple_form($context) {
    // This is a value only -- equivalent to a hidden field, except
    // that it is never rendered into the HTML.
    $form = array();  
    // This creates a fieldset for the body:
    $form['admin_messages'] = array(
        '#type' => 'fieldset',
        '#title' => t('Compose New Message For Users'),
    );
     $form['to'] = array(
      '#type'               => 'textfield',
      '#title'              => t('To'),
      '#description'        => t('Separate multiple names with commas.'),
      '#required'           => TRUE,
      '#weight'             => -10,
      '#size'               => 50,
      '#autocomplete_path'  => 'admin/settings/admin_messaging_multiple_autocomplete',
      // Do not hardcode #maxlength, make it configurable by number of recipients, not their name length.
    );   

    // Textfield for subject of the body
    $form['admin_messages']['subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#size' => 50,
        '#maxlengh' => 255,
        '#required' => true,
        '#description' => t('The subject of the message.'),
    );
    // Text area for the body.
    $form['admin_messages']['body'] = array(
        '#type' => 'textarea',
        '#title' => t('Message'),
        '#cols' => 50,
        '#rows' => 5,
        '#required' => true,
        '#description' => t('The body of the message.'),
    );
    // Creates a fieldset for details
    // $form['details'] = array(
        // '#type' => 'fieldset',
        // '#title' => t("Details"),
    // );
    // Creates a fieldset for sending options
	$method_list = messaging_method_list();
	$method_list['debug'] = 'Debug';
	$method_list['user'] = 'User Prefered';
	$form['method'] = array (
		'#type' => 'select' ,
		'#title' => t('Select Sending Method') ,
		 '#rows' => 5 ,
		'#options' => $method_list, //the option list called from the messaging list, alternate '#options' => array(email => 'Email', web => 'Web') ),
		'#default_value' => messaging_method_default(),
	) ;
  // submit button:
  $form['submit_messages'] = array(
    '#type' => 'submit',
    '#value' => t('Send Message'),
  );
  return ($form);
}
/**
 * Implements hook_mail_alter().
 */
function admin_messaging_mail_alter(&$message) {
    $append = "\n=======================\n"
        ."Message sent by the administrator from !site_name (!website). \n"
        ."If you believe this message to be a case of abuse, \n"
        ."please contact !site_email.\n";
    $args = array(
        '!website' => url('', array('absolute' => true)),
        '!site_email' => variable_get('site_mail', null),
        '!site_name' => variable_get('site_name', 'Unknown'),
    );
    $message['body'] .= t($append, $args);
}
        
/**
 * Form submission handler for constructor function admin_messaging_compose_form()
 */
function admin_messaging_multiple_form_submit($form, &$form_state) {
  // $acount = user_load($form_state['values']['to']->uid);
	$user_list = explode(',', $form_state['values']['to']);
	foreach ($user_list as $key => $value) {
		if ($value == ' ' || $value == null)
			unset($user_list[$key]);
	}
  global $user;
  
  $form_values = $form_state['values'];
  $body = split("\n", $form_state['values']['body']);
  $body = array_map('trim', $body);  
  $message = array (
    'type' => 'push',
    'subject' => $form_state['values']['subject'],
		'body' => $body,  
  );
	
	// If method defined force sending method
	if ($form_state['values']['method'] != 'user')
		$method = $form_state['values']['method'];
	else
		$method = null;	
	
	// Set redirect page
  // $form_state['redirect'] = sprintf('admin/settings/admin-messaging-multiple');
	
	// send message if user set
  // if ($account) 
  foreach ($user_list as $key => $value) {
		$account = user_load(array('name' => trim($value)));
		// print_r($account);
		$result = messaging_message_send_user($account, $message, $method);
	}
   //}
	   
    drupal_set_message(t('%result', array('%result' => $result ? 'Message sent successfully' : 'There was an error message sending failed')));
}

