<?php

// Authentication url.
define('ALMASRY_GDATA_AUTHENTICATE_URL', 'https://www.google.com/youtube/accounts/ClientLogin');

// Url for upload tokens
define('ALMASRY_GDATA_UPLOAD_TOKEN', 'http://gdata.youtube.com/action/GetUploadToken');

// Upload URL
define('ALMASRY_UPLOAD_URL', 'http://uploads.gdata.youtube.com/feeds/api/users/default/uploads');


function youtube_direct_upload_page() {
  // retrive youtube object, it is a must to initiate the connection
  $yt = almasry_gdata_youtube(TRUE);
  $output = '';

  // if we just uploaded a video, show file status to user
  if (isset($_GET['status']) && isset($_GET['id'])) {
    $status = $_GET['status'];
    $id = $_GET['id'];


    // status 200 means  upload is succeeded, but still
    // the video might be rejected due to file type, or duplication or any other reason
    if ($status == '200') {
      $videoEntry = $yt->getVideoEntry($id);
      $output .= 'Upload completed successfully. You can access the video ' . l('here', $videoEntry->getVideoWatchPageUrl())
      . '<br> <br>';

      try {
        $control = $videoEntry->getControl();
      } catch (Zend_Gdata_App_Exception $e) {
        drupal_set_message('couldnot get control object');
      }

      if ($control instanceof Zend_Gdata_YouTube_Extension_Control) {
        $state = $control->getState();

        if ($state instanceof Zend_Gdata_YouTube_Extension_State) {
          $output .= 'Upload status for video ID "' . $videoEntry->getVideoId() . '" is ' .
          $state->getName() . ' - ' . $state->getText() . "\n";
        } else {
          $output .= "Not able to retrieve the video status information yet. " .
            "Please try again later.\n";
        }
      }
    }
    else {
      drupal_set_message('video upload failed');
    }
  }

  // create a new VideoEntry object
  $myVideoEntry = new Zend_Gdata_YouTube_VideoEntry();

  $myVideoEntry->setVideoTitle('Almasry browser upload');
  $myVideoEntry->setVideoDescription('Almasry broswer upload');

  // The category must be a valid YouTube category!
  // we should use something like News
  $myVideoEntry->setVideoCategory('Autos');

  // Set keywords. Please note that this must be a comma-separated string
  // and that individual keywords cannot contain whitespace
  $myVideoEntry->SetVideoTags('news, egypt, citizin, journalism');

  $tokenArray = $yt->getFormUploadToken($myVideoEntry, ALMASRY_GDATA_UPLOAD_TOKEN);
  $tokenValue = $tokenArray['token'];
  $postUrl = $tokenArray['url'];


  // place to redirect user after upload
  $nextUrl = 'http://localhost/masry7/youtube/browser/upload';

  // build the form
  $output .= '<br><form action="'. $postUrl .'?nexturl='. $nextUrl .
    '" method="post" enctype="multipart/form-data">'. 
    '<input name="file" type="file"/>'. 
    '<input name="token" type="hidden" value="'. $tokenValue .'"/>'.
    '<input value="Upload Video File" type="submit" />'. 
    '</form>';

  return $output;
}

/**
 * upload a video to youtube
 * Enter description here ...
 * @param $video
 */
function youtube_upload_video($video) {
  static $http_client;
  if (!$http_client) {
    $http_client = almasry_authenticate_youtube();
  }

  $yt = almasry_gdata_youtube($http_client);
  if (!$yt) {
    return FALSE;
  }

  // create a new VideoEntry object
  $myVideoEntry = new Zend_Gdata_YouTube_VideoEntry();

  // making video private, we should make it public when the node containing it is published
  // $myVideoEntry->setVideoPrivate();
  
  // make the video unlisted
  $unlisted = new Zend_Gdata_App_Extension_Element('yt:accessControl', 'yt', 'http://gdata.youtube.com/schemas/2007', '');
  $unlisted->setExtensionAttributes(array(
    array('namespaceUri' => '', 'name' => 'action', 'value' => 'list'),
    array('namespaceUri' => '', 'name' => 'permission', 'value' => 'denied')
  ));
  $myVideoEntry->setExtensionElements(array($unlisted));

  // create a new Zend_Gdata_App_MediaFileSource object
  $filesource = $yt->newMediaFileSource($video->path);
  $filesource->setContentType($video->filemime);

  // set slug header
  $filesource->setSlug($video->filename);

  // add the filesource to the video entry
  $myVideoEntry->setMediaSource($filesource);

  $myVideoEntry->setVideoTitle($video->title);
  $myVideoEntry->setVideoDescription('almasryalyoum video');

  // The category must be a valid YouTube category!
  $myVideoEntry->setVideoCategory('Autos');

  // TODO Set keywords. Please note that this must be a comma-separated string
  // and that individual keywords cannot contain whitespace
  $myVideoEntry->SetVideoTags('cars, funny');


  // try to upload the video, catching a Zend_Gdata_App_HttpException,
  // if available, or just a regular Zend_Gdata_App_Exception otherwise
  try {
    $newEntry = $yt->insertEntry($myVideoEntry, ALMASRY_UPLOAD_URL, 'Zend_Gdata_YouTube_VideoEntry');
 
    // Set API version to 2.0.
    $newEntry->setMajorProtocolVersion(2);

    if ($newEntry) {
      $video = new stdClass();
      $video->id = $newEntry->getVideoId();
      $video->url = $newEntry->getVideoWatchPageUrl();
      
      return $video;
    }
  } catch (Zend_Gdata_App_HttpException $httpException) {
    watchdog('misc', $httpException->getRawResponseBody(), null, 'error');
  } catch (Zend_Gdata_App_Exception $e) {
    watchdog('misc', $e->getMessage(), null, 'error');
  }

  return FALSE;
}


function youtube_upload_video_status($id) {
  static $http_client;
  if (!$http_client) {
    $http_client = almasry_authenticate_youtube();
  }

  $yt = almasry_gdata_youtube($http_client);
  if (!$yt) {
    return 0;
  }

  $videoEntry = $yt->getVideoEntry($id);

  try {
    $control = $videoEntry->getControl();
  } catch (Zend_Gdata_App_Exception $e) {
    watchdog('misc', 'Misc: error %error while attempting to upload a video.', array('%error' => $e->getMessage()));
  }
  
  $status = new stdClass();
  if (is_object($videoEntry) && $control === NULL) {
    $status->status = 2;
    $status->upload_status = 1;
    $status->upload_status_reason = '';
    return $status;
  }

  if ($control instanceof Zend_Gdata_YouTube_Extension_Control) {
    $state = $control->getState();

    if ($state instanceof Zend_Gdata_YouTube_Extension_State) {
      switch ($state->getName()) {
        // those statuses indicates failer: restricted, deleted, rejected and failed.
        case 'restricted' :
        case 'deleted':
        case 'rejected':
        case 'failed':
          $status->status = 2;  // finished
          $status->upload_status = 0; // rejected
          $status->upload_status_reason = $state->getName() . ' - ' . $state->getText(); // reason
          return $status;
          
        // this status means wait .. no result yet
        case 'processing':
          $status->status = 1;  // finished
          $status->upload_status = 0; // rejected
          $status->upload_status_reason = 'processing'; // reason
          return $status;
          
          
        default:
          // nothing to do here
          return NULL;
      }
    }
    else {
      // FIXME don't know what this means
    }
  }

  return;
}


/**
 * Wrapper function to include necessary Zend Gdata functions and
 * libraries.
 */
function almasry_gdata_initialize($op) {
  // Include Zend loader class.
  if (@include_once('Zend/Loader.php')) {
    switch ($op) {
      case 'authenticate' :
        Zend_Loader::loadClass('Zend_Gdata_ClientLogin');
        break;
      case 'youtube' :
        Zend_Loader::loadClass('Zend_Gdata_YouTube');
        break;
    }
    return TRUE;
  }
  else {
    watchdog('almasry', "Couldn't find the Zend client libraries.", array(), WATCHDOG_ERROR);
  }
}


/**
 * Authenticate YouTube user account.
 */
function almasry_authenticate_youtube() {

  $username = 'obad2015@gmail.com';
  $password = 'Mr-Abdelrahman';
  $developer_key = 'AI39si5rPmsZtYv6XETZAEjlOyIRG2QEVtgjdftjjufCqleFj6TZrI178sJNKy-IJ831cjj_VPgviVasjct4hdlZxHdrQBYxBA';

  if (almasry_gdata_initialize('authenticate')) {
    try {
      $http_client = Zend_Gdata_ClientLogin::getHttpClient(
      $username, // username
      $password, // password
      $service = 'youtube',
      $client = NULL,
      $source = 'almasry',
      $loginToken = NULL,
      $loginCaptcha = NULL,
      ALMASRY_GDATA_AUTHENTICATE_URL
      );
    }
    catch (Zend_Gdata_App_AuthException $authEx) {
      drupal_set_message(t('YouTube uploads currently unavailable'), 'error');
      watchdog('almasry', 'Authentication error for YouTube Account', array(), WATCHDOG_ERROR);
      return FALSE;
    }
    catch (Zend_Gdata_App_HttpException $e) {
      drupal_set_message(t('YouTube uploads currently unavailable'), 'error');
      watchdog('almasry', 'Authentication error for YouTube Account: %error', array('%error' => $e->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  else {
    return FALSE;
  }

  // Pass developer key.
  $http_client->setHeaders('X-GData-Key', 'key=' . $developer_key);

  return $http_client;
}



/**
 * Create a Zend YouTube object.
 */
function almasry_gdata_youtube($http_client = NULL) {
  if ($http_client === TRUE) {
    $http_client = almasry_authenticate_youtube();
  }

  almasry_gdata_initialize('youtube');
  try {
    $yt = new Zend_Gdata_YouTube($http_client);
    // Set API version to 2.0.
    $yt->setMajorProtocolVersion(2);
  }
  catch (Zend_Gdata_App_HttpException $e) {
    watchdog('almasry', 'Authentication error while creating a YouTube connection object: %error', array('%error' => $e->getMessage()), WATCHDOG_ERROR);
  }
  return $yt;
}

