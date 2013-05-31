<?php

/**
 * @file
 * Public page callbacks for the Pay with a Tweet module.
 *
 * @ingroup pay_with_a_tweet
 */

/**
 * Menu callback. Connect to Twitter to grant access to publish tweet.
 * 
 * @global $base_url
 * @param $pay_with_a_tweet
 *   The pay_with_a_tweet entity to use.
 */
function pay_with_a_tweet_twitter_connect($pay_with_a_tweet) {
  global $base_url;

  $_SESSION['oauth_pid'] = $pay_with_a_tweet->pid;

  $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());

  /* Build TwitterOAuth object with client credentials. */
  $connection = new TwitterOAuth($settings['consumer_key'], $settings['consumer_secret']);

  /* Get temporary credentials. */
  $request_token = $connection->getRequestToken($base_url . '/pay_with_a_tweet/' . $pay_with_a_tweet->pid . '/callback');

  /* Save temporary credentials to session. */
  $_SESSION['oauth_token'] = $token = $request_token['oauth_token'];
  $_SESSION['oauth_token_secret'] = $request_token['oauth_token_secret'];
  /* If last connection failed don't display authorization link. */
  switch ($connection->http_code) {
    case 200:
      /* Build authorize URL and redirect user to Twitter. */
      $url = $connection->getAuthorizeURL($token);
      header('Location: ' . $url);
      break;

    default:
      /* Show notification if something went wrong. */
      watchdog('Pay with a Tweet', 'Twitter connect error %error in button %title: %message', array('%title' => $pay_with_a_tweet->title . ' [' . $pay_with_a_tweet->pid . ']',
        '%error' => $connection->http_code, '%message' => $connection->http_header['status']), WATCHDOG_CRITICAL, $base_url . '/pay_with_a_tweet/' . $pay_with_a_tweet->pid);

      drupal_set_message(t('Could not connect to Twitter. Refresh the page or try again later.'), 'error');
      drupal_goto('<front>');
      drupal_exit();
  }
}

/**
 * Menu callback. Publish the tweet and send the download.
 * 
 * @global $base_url
 * @param $pay_with_a_tweet
 *   The pay_with_a_tweet entity to use.
 */
function pay_with_a_tweet_twitter_callback($pay_with_a_tweet) {
  global $base_url;

  $_SESSION['oauth_pid'] = $pay_with_a_tweet->pid;
  $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());

  /* If the oauth_token is old redirect to the connect page. */
  if (isset($_REQUEST['oauth_token']) && $_SESSION['oauth_token'] !== $_REQUEST['oauth_token']) {
    unset($_SESSION['oauth_token']);
    unset($_SESSION['oauth_token_secret']);
    drupal_set_message(t('The oauth_token is old'), 'error');
    drupal_goto('<front>');
    drupal_exit();
  }

  /* Create TwitteroAuth object with app key/secret and token key/secret from default phase */
  $connection = new TwitterOAuth($settings['consumer_key'], $settings['consumer_secret'], $_SESSION['oauth_token'], $_SESSION['oauth_token_secret']);

  /* Request access tokens from twitter */
  $access_token = $connection->getAccessToken($_REQUEST['oauth_verifier']);

  if ($connection->http_code != 200) {

    watchdog('Pay with a Tweet', 'Twitter callback connect error %error in button %title: %message', array('%title' => $pay_with_a_tweet->title . ' [' . $pay_with_a_tweet->pid . ']',
      '%error' => $connection->http_code, '%message' => $connection->http_header['status']), WATCHDOG_CRITICAL, $base_url . '/pay_with_a_tweet/' . $pay_with_a_tweet->pid . '/callback');

    drupal_set_message(t('Could not connect to Twitter. Refresh the page or try again later.'), 'error');
    drupal_goto('<front>');
    drupal_exit();
  }

  $connection_publish = new TwitterOAuth($settings['consumer_key'], $settings['consumer_secret'], $access_token['oauth_token'], $access_token['oauth_token_secret']);
  $twitter_user = $connection_publish->get('account/verify_credentials');

  $status = $connection_publish->post('statuses/update', array('status' => $pay_with_a_tweet->message));

  /* Remove no longer needed request tokens */
  unset($_SESSION['oauth_token']);
  unset($_SESSION['oauth_token_secret']);
  unset($_SESSION['oauth_pid']);

  /* If HTTP response is 200 continue otherwise send to connect page to retry */
  if ($connection_publish->http_code == 200) {

    $mime = 'application/force-download';
    $file = file_load($pay_with_a_tweet->download);
    $content = file_get_contents(drupal_realpath($file->uri));
    /* Show notification if something went wrong. */
    watchdog('Pay with a Tweet', 'Tweet publish in %screen_name correctly. Download %title', array('%title' => $pay_with_a_tweet->title . ' [' . $pay_with_a_tweet->pid . ']',
       '%screen_name' => '@' . $twitter_user->screen_name), WATCHDOG_INFO, $base_url . '/pay_with_a_tweet/' . $pay_with_a_tweet->pid . '/callback');

    header('Content-type: ' . $file->filemime);
    header("Content-Description: File Transfer");
    header("Content-Length: " . $file->filesize);
    header('Content-Disposition: attachment; filename="' . $file->filename . '"');

    print $content;
    drupal_exit();
  }
  else {
    /* Show notification if something went wrong. */
    watchdog('Pay with a Tweet', 'Tweet publish error %error in button %title: %message', array('%title' => $pay_with_a_tweet->title . ' [' . $pay_with_a_tweet->pid . ']',
      '%error' => $connection_publish->http_code, '%message' => $connection_publish->http_header['status']), WATCHDOG_CRITICAL, $base_url . '/pay_with_a_tweet/' . $pay_with_a_tweet->pid . '/callback');

    drupal_set_message(t('Could not publish in Twitter. Refresh the page or try again later.'), 'error');
    drupal_goto('<front>');
    drupal_exit();
  }
}