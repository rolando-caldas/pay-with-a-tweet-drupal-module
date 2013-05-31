<?php

/**
 * @file
 * pay_with_a_tweet entity class for the Pay with a Tweet module.
 *
 * @ingroup pay_with_a_tweet
 */

class PayWithATweetController extends DrupalDefaultEntityController {

  public function save($pay_with_a_tweet) {
    if (empty($pay_with_a_tweet->pid)) {
      drupal_write_record('pay_with_a_tweet', $pay_with_a_tweet);
    }
    else {
      drupal_write_record('pay_with_a_tweet', $pay_with_a_tweet, 'pid');
    }

    /* @TODO future support to extra fields */
    field_attach_insert('pay_with_a_tweet', $pay_with_a_tweet);
    module_invoke_all('entity_insert', 'pay_with_a_tweet', $pay_with_a_tweet);
    return $pay_with_a_tweet;
  }

  public function delete($pids) {
    if (!empty($pids) && is_array($pids)) {
      $pwt = pay_with_a_tweet_load_multiple($pids);
      $transaction = db_transaction();

      try {
        foreach ($pwt AS $pay_with_a_tweet) {
          module_invoke_all('pay_with_a_tweet_delete', $pay_with_a_tweet);
          //module_invoke_all('entity_delete', $pay_with_a_tweet, 'pay_with_a_tweet');
          /* @TODO future support to extra fields */
          field_attach_delete('pay_with_a_tweet', $pay_with_a_tweet);
        }

        db_delete('pay_with_a_tweet')
            ->condition('pid', $pids, 'IN')
            ->execute();
      } catch (Exception $e) {
        $return = FALSE;
        $transaction->rollback();
        watchdog_exception('pay_with_a_tweet', $e);
        throw $e;
      }

      $this->resetCache();
      $return = TRUE;
    }
    else {
      $return = FALSE;
    }
    return $return;
  }

}