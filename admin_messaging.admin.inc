<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the admin_messaging module.
 *
 * @ingroup admin_messaging
 */
 
/**
 * Form constructor function with the fields interface for autocomplete users
 */
function admin_messaging_multiple_form($context, $account) {
    // This is a value only -- equivalent to a hidden field, except
    // that it is never rendered into the HTML.
	$recipient_addresses = array();
	  $result = db_query('SELECT mail FROM {users} WHERE uid > 0');
        while($account = db_fetch_object($result)) {
          $recipient_addresses[] = $account->mail;
        }
    $form = array();
    $form['to'] = array(
        '#type' => 'value',
        '#value' => $account,
    );     
    // This creates a fieldset for the body:
    $form['admin_messages'] = array(
        '#type' => 'fieldset',
        '#title' => t('Compose New Message For Users'),
    );
     $form['admin_messages']['recipient'] = array(
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
 * Compose a message.
 * Creating form neccessary to compose message for.
 * To be send to multiple users.
 * @param $to
 *  The address to send to.
 * @return
 *  HTML.
 */
function admin_messaging_multiple_compose($user) {
    //print_r($account);
    global $user;
	$recipient_addresses = array();
	if ($user->uid <= 0)
        return t('User ID must be an integer.');
		$account = $user;
		$result = db_query('SELECT mail FROM {users} WHERE uid > 0');
               while($account = db_fetch_object($result)) {
                 $recipient_addresses[] = $account->mail;
                  Foreach ($recipient_addresses as $account) {	
	            $to = $account->mail;
		      $sb = '<p>'
                    .t('Send message to users', array('@email' => $to))
                    .'</p>';
                 $sb .= drupal_get_form('admin_messaging_multiple_form', $account);
    return $sb;
	}
    }
  }
    
/**
 * Form submission handler for constructor function admin_messaging_compose_form()
 */
function admin_messaging_multiple_form_submit($form, &$form_state) {
	//print_r($form_state);
  // $acount = user_load($form_state['values']['to']->uid);
  global $user;
  
  $form_values = $form_state['values'];
  $account = $form_state['values']['to'];
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
  $form_state['redirect'] = sprintf('admin/settings/admin-messaging-multiple');
	
	// send message if user set
  if ($account) {
		$result = messaging_message_send_user($account, $message, $method);
	}
   drupal_set_message(t('Sent message with result: %result', array('%result' => $result ? 'OK' : 'Error'))); /**/
}

