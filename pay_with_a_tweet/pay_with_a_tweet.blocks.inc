<?php

/**
 * @file
 * Blocks functions and hooks for the Pay with a Tweet module.
 * 
 * @ingroup pay_with_a_tweet
 */

/**
 * Implements hook_block_info().
 * 
 */
function pay_with_a_tweet_block_info() {
  $block = array();

  $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());

  if ($settings['own_blocks']) {
    $entities = pay_with_a_tweet_get_all();
    if (is_array($entities)) {
      foreach ($entities AS $entity) {
        $block[PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-' . $entity->pid]['info'] = t('Pay with a tweet - !title', array('!title' => $entity->title));
        $block[PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-' . $entity->pid]['cache'] = DRUPAL_CACHE_GLOBAL;
      }
    }
  }

  for ($i = 0; $i < $settings['blocks']; $i++) {
    $block[PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . $i]['info'] = t('Pay with a tweet - block') . ' ' . $i;
    $block[PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . $i]['cache'] = DRUPAL_CACHE_GLOBAL;
  }

  return $block;
}

/**
 * Implements hook_block_configure().
 * 
 */
function pay_with_a_tweet_block_configure($delta = '') {
  $form = array();
  if (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', $delta)) {

    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());
    $item = 'own-' . intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', '$1', $delta));

    $form['pay_with_a_tweet']['pay_with_a_tweet_show'] = array(
      '#type' => 'select',
      '#title' => t('Show the text'),
      '#options' => array('0' => 'After button', '1' => 'Before button'),
      '#default_value' => isset($settings['blocks-config'][$item]['show']) ? $settings['blocks-config'][$item]['show'] : '0'
    );

    $form['pay_with_a_tweet']['pay_with_a_tweet_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Extra info to show in the block'),
      '#default_value' => isset($settings['blocks-config'][$item]['text']) ? $settings['blocks-config'][$item]['text'] : ''
    );
  }
  elseif (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', $delta)) {

    $item = intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', '$1', $delta));
    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());

    $entities = pay_with_a_tweet_get_all();
    $options = array();
    if (is_array($entities)) {
      foreach ($entities AS $value) {
        $options[$value->pid] = $value->title;
      }
    }
    $form['pay_with_a_tweet']['pay_with_a_tweet_pid'] = array(
      '#type' => 'select',
      '#title' => t('Select the button to show'),
      '#options' => $options,
      '#default_value' => isset($settings['blocks-config'][$item]['pid']) ? $settings['blocks-config'][$item]['pid'] : ''
    );

    $form['pay_with_a_tweet']['pay_with_a_tweet_show'] = array(
      '#type' => 'select',
      '#title' => t('Show the text'),
      '#options' => array('0' => 'After button', '1' => 'Before button'),
      '#default_value' => isset($settings['blocks-config'][$item]['show']) ? $settings['blocks-config'][$item]['show'] : '0'
    );

    $form['pay_with_a_tweet']['pay_with_a_tweet_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Extra info to show in the block'),
      '#default_value' => isset($settings['blocks-config'][$item]['text']) ? $settings['blocks-config'][$item]['text'] : ''
    );
  }
  return $form;
}

/**
 * Implements hook_block_save().
 * 
 */
function pay_with_a_tweet_block_save($delta = '', $edit = array()) {

  if (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', $delta)) {

    $pid = intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', '$1', $delta));
    $item = 'own-' . $pid;
    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());
    $new_settings = $settings;

    $new_settings['blocks-config'][$item] = array('pid' => $pid,
      'text' => check_plain($edit['pay_with_a_tweet_text']),
      'show' => check_plain($edit['pay_with_a_tweet_show']));

    variable_set(PAY_WITH_A_TWEET, array_merge($settings, $new_settings));
  }
  elseif (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', $delta)) {

    $item = intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', '$1', $delta));
    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());
    $new_settings = $settings;

    $new_settings['blocks-config'][$item] = array('pid' => intval($edit['pay_with_a_tweet_pid']),
      'text' => check_plain($edit['pay_with_a_tweet_text']),
      'show' => check_plain($edit['pay_with_a_tweet_show']));

    variable_set(PAY_WITH_A_TWEET, array_merge($settings, $new_settings));
  }
}

/**
 * Implements hook_block_view().
 * 
 */
function pay_with_a_tweet_block_view($delta = '') {
  $block = array();
  $view = FALSE;
  $pid = FALSE;

  if (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', $delta)) {
    $pid = intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . 'own-([\d]+)$#', '$1', $delta));
    $item = 'own-' . $pid;

    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());
    $config = isset($settings['blocks-config'][$item]) ? $settings['blocks-config'][$item] : _pay_with_a_tweet_block_config($pid);
    $view = TRUE;
  }
  elseif (preg_match('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', $delta)) {

    $item = intval(preg_replace('#^' . PAY_WITH_A_TWEET_BLOCK_SHORTCUTS . '([\d]+)$#', '$1', $delta));

    $settings = variable_get(PAY_WITH_A_TWEET, _pay_with_a_tweet_config());
    $config = isset($settings['blocks-config'][$item]) ? $settings['blocks-config'][$item] : _pay_with_a_tweet_block_config($item);
    $pid = !empty($config) ? $config['pid'] : FALSE;
    $view = TRUE;
  }

  if ($view && $pid) {
    $pay_with_a_tweet = pay_with_a_tweet_load($pid);

    if (!empty($pay_with_a_tweet)) {
      $css_to_load = NULL;
      $path = drupal_get_path('module', 'pay_with_a_tweet');
      $path_to_theme = path_to_theme();

      $path_css = array();
      $path_css[] = $path_to_theme . '/pay_with_a_tweet.css';
      $path_css[] = $path . '/css/blocks.css';

      foreach ($path_css AS $value) {
        if (file_exists($value)) {
          $css_to_load = $value;
          break;
        }
      }
      drupal_add_css($css_to_load, 'screen');

      pay_with_a_tweet_render($pay_with_a_tweet);

      $block['content'] = theme('pay_with_a_tweet_button', array('pay_with_a_tweet' => $pay_with_a_tweet, 'configuration' => (object) $config));
    }
  }
  return $block;
}
