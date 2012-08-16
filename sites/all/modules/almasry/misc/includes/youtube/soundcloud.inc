<?php

// Client ID for app created on my personal account
define('CLIENT_ID', '592200392b3f19b0ce9457a984778705');

// Client Secret for app 
define('CLIENT_SECRET', '866543fee3e7c2b790f76c540e5b7865');

// soundcloud redirect URL
define('REDIRECT_URL', 'http://localhost/masry7');

// those two elements should NEVER be saved like this, this is for testing purposes only
define('ACCESS_TOKEN', '1-22898-14519136-e383c7203b6b5b0');
define('REFRESH_TOKEN', '1c0ffe8ead3dc797d594fb033ce68d57');


function almasry_authenticate_soundcloud() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object with app credentials
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, REDIRECT_URL);
  
  // redirect user to authorize URL
  drupal_goto($client->getAuthorizeUrl(), array('external' => TRUE));
}


function almasry_authenticate_soundcloud_callback() { 
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object with app credentials
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, REDIRECT_URL);

  $code = $_GET['code'];
  
  $access_token = $client->accessToken($code); 
}


function almasry_authenticated_username_soundcloud() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object and set access token
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET);
  $client->setAccessToken(ACCESS_TOKEN);
  
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
//   static $client;
//   if (is_object($client)) {
//     return $client;
//   }
  
  //almasry_soundcloud_refresh_token();
  
  // create client object and set access token
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET);
  $client->setAccessToken(ACCESS_TOKEN);
  
  return $client;
}

/*
 * Refresh almasry access tokens, this code is borken
 * needs to be finished
 */
function almasry_soundcloud_refresh_token() {
  include_once libraries_get_path('php-soundcloud') . '/Services/Soundcloud.php' ;
  
  // create client object with app credentials
  $client = new Services_Soundcloud(CLIENT_ID, CLIENT_SECRET, REDIRECT_URL);
  
  $new_access_token = $client->accessTokenRefresh(REFRESH_TOKEN);
  
  $access_token = $new_access_token['access_token'];
  $refresh_token = $new_access_token['refresh_token'];
}



