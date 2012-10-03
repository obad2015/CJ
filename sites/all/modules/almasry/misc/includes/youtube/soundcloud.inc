<?php

// Client ID for app created on my personal account
define('CLIENT_ID', '592200392b3f19b0ce9457a984778705');

// Client Secret for app 
define('CLIENT_SECRET', '866543fee3e7c2b790f76c540e5b7865');

// those two elements should NEVER be saved like this, this is for testing purposes only
define('ACCESS_TOKEN', '1-22898-14519136-e383c7203b6b5b0');
define('REFRESH_TOKEN', '1c0ffe8ead3dc797d594fb033ce68d57');


function almasry_authenticate_soundcloud() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object with app credentials
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, REDIRECT_URL);
  
  // redirect user to authorize URL
  drupal_goto($client->getAuthorizeUrl(array('score' => 'non-expiring')), array('external' => TRUE));
}


function almasry_authenticate_soundcloud_callback() { 
  global $base_url;
  global $user;
  
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object with app credentials
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, $base_url . '/admin/config/system/cj/soundcloud-callback');

 
  // get the access code and save it into variables
  $code = $_GET['code'];
  $tokens = $client->accessToken($code);
  
  variable_set('soundcloud_access_token', $tokens['access_token']);
  if (array_key_exists('refresh_token', $tokens)) {
    variable_set('soundcloud_refresh_token', $tokens['refresh_token']);
  }
  
  if ($user->uid == 1) {
    drupal_set_message('access code token is updated to ' . $tokens['access_token']);
  }
  
  drupal_goto(CJ_ADMIN_PATH);
}


function almasry_soundcloud_authenticated_username() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object and set access token
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET);
  $client->setAccessToken(variable_get('soundcloud_access_token', ''));
  
  // make an authenticated call
  $current_user = json_decode($client->get('me'));
  print $current_user->username;
}

/**
 * upload file to soundcloud
 * @param $file object with title, path keys about the file to upload
 * @return $file_id the audio track ID on soundcloud
 */
function almasry_soundcloud_upload_audio($file) {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object
  $client = almasry_soundcloud_client();
  
  // upload audio file
  $track = json_decode($client->post('tracks', array(
      'track[title]' => $file->title,
      'track[asset_data]' => '@' . drupal_realpath($file->uri)
  )));
  
  // return id and url
  $audio = new stdClass();
  $audio->id = $track->id;
  $audio->url = $track->permalink_url;
  
  return $audio;
}


/**
 * check uploaded track status
 * 
 * @param int $id the track id to check its status
 * @return int 0 for failer 1 for passed 
 */
function almasry_soundcloud_upload_audio_status($id) {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object
  $client = almasry_soundcloud_client();
  
  // try to get a track
  try {
    $track = json_decode($client->get('tracks/' . $id));
  } 
  catch (Services_Soundcloud_Invalid_Http_Response_Code_Exception $e) {
    watchdog('Misc', $e->getMessage());
    return NULL;
  }
  
  //$track = (object)track;
  print_r($track);
  
  if (isset($track->state) && $track->state == 'finished') {
     $status->status = 2;  // finished
     $status->upload_status = 1; // accepted
     $status->upload_status_reason = 'Finished'; // reason
     return $status;
  }
  
  return NULL;
}

/**
 * Retrieves soundcloud client object
 * @return Services_Soundcloud object
 */
function almasry_soundcloud_client() {
  almasry_soundcloud_refresh_token();
  
  // create client object and set access token
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET);
  $client->setAccessToken(variable_get('soundcloud_access_token', ''));
  
  return $client;
}

/*
 * Refresh almasry access tokens, this code is borken
 * needs to be finished
 */
function almasry_soundcloud_refresh_token() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  $refresh_token = variable_get('soundcloud_refresh_token', NULL);
  
  // we can only refresh if we have previous refresh token
  if ($refresh_token) {
    // create client object with app credentials
    $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, REDIRECT_URL);
    
    $new_tokens = $client->accessTokenRefresh(variable_get('soundcloud_refresh_token', ''));
    
    variable_set('soundcloud_access_token', $new_tokens['access_token']);
    variable_set('soundcloud_refresh_token', $new_tokens['refresh_token']);
  }
}



