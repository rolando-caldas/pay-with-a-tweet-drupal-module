<?php

/**
 * @file
 * Token hooks for the Pay with a Tweet module.
 *
 * @ingroup pay_with_a_tweet
 */

/**
 * Implements hook_token_info().
 *
 */
function pay_with_a_tweet_token_info() {
  $types['pay_with_a_tweet'] = array(
    'name' => t("Pay with a Tweet Tokens"),
    'description' => t("Tokens for Pay with a Tweet buttons."),
  );

  $pay_with_a_tweet['pid'] = array(
    'name' => t("Button"),
    'description' => t("Put the pid instead ? to show the Pay with a tweet button."),
    'dynamic' => TRUE,
  );

  return array(
    'types' => $types,
    'tokens' => array(
      'pay_with_a_tweet' => $pay_with_a_tweet,
    ),
  );
}

/**
 * Implements hook_tokens(). 
 *
 */
function pay_with_a_tweet_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  if ($type == 'pay_with_a_tweet') {
    $dynamic_tokens = token_find_with_prefix($tokens, 'pid');
    foreach ($dynamic_tokens AS $pid => $original) {
      $replacements[$original] = pay_with_a_tweet_token_render($pid);
    }
  }
  return $replacements;
}

/**
 * Token HTML result
 *
 * @param $pid
 *   pid to load pay_with_a_tweet entity
 * @return
 *   HTML to print of the pay_with_a_tweet entity
 */
function pay_with_a_tweet_token_render($pid) {
  $pay_with_a_tweet = pay_with_a_tweet_load($pid);

  pay_with_a_tweet_render($pay_with_a_tweet);

  return theme('pay_with_a_tweet_token', array('pay_with_a_tweet' => $pay_with_a_tweet));
}