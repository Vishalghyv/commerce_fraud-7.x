<?php

/**
 * @file
 * Default fraud rules.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_fraud_default_rules_configuration() {
  $rules = array();

  $rule = rules_reaction_rule();
  $rule->label = t('Change order status depending on fraud level');
  $rule->tags = array('Commerce Fraud');
  $rule->weight = 100;
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_compare_fraud_level', array(
      'commerce_order:select' => 'commerce_order',
      'commerce_fraud_level_function' => 'commerce_fraud_is_blacklisted',
    ))
    ->action('commerce_order_update_status', array(
      'commerce_order:select' => 'commerce_order',
      'order_status' => 'fraudulent',
    ));
  $rules['commerce_fraud_update_order_status'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score if order has x amount of products.');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_order_compare_total_product_quantity', array(
      'commerce_order:select' => 'commerce_order',
      'operator' => '>',
      'value' => 10,
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Order has more than 10 products'),
    ));
  $rules['commerce_fraud_item_quantity'] = $rule;

  if (module_exists('ip2country')) {
    $rule = rules_reaction_rule();
    $rule->label = t('Increase fraud score if user IP is not in selected countries');
    $rule->tags = array('Commerce Fraud');
    $rule->active = FALSE;
    $rule
      ->event('commerce_checkout_complete')
      ->condition(rules_condition('commerce_fraud_user_ip_in_country', array(
        'countries' => 'CA\r\nUS',
      ))->negate())
      ->action('commerce_fraud_increase_fraud_count', array(
        'commerce_order:select' => 'commerce_order',
        'counter' => 5,
        'description' => t('User IP not in USA or Canada'),
      ));
    $rules['commerce_fraud_country_ip'] = $rule;
  }

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score for orders with po box');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_has_po_box', array(
      'commerce_order:select' => 'commerce_order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Shipping Address has po box'),
    ));
  $rules['commerce_fraud_has_po_box'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score for orders over x price');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_price_compare_price', array(
      'first_price:select' => 'commerce-order:commerce-order-total',
      'operator' => '>',
      'second_price' => array(
        'amount' => 50000,
        'currency_code' => commerce_default_currency(),
      ),
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Order is greater than $500.00'),
    ));
  $rules['commerce_fraud_order_total'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score for orders placed within x minutes');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_last_order', array(
      'commerce_order:select' => 'commerce-order',
      'minutes' => '5',
      'order_status' => array('completed'),
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Previous order completed within 5 minutes'),
    ));
  $rules['commerce_fraud_last_order'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score if order is placed by anonymous user');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_anonymous_user', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Order placed by anonymous user'),
    ));
  $rules['commerce_fraud_anonymous_user'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Check Email with fraudulent order');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_email', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Email is same as one of fraudulent orders'),
    ));
  $rules['commerce_fraud_check_email'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Check Uid with fraudulent order');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_uid', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Uid is same as one of fraudulent orders'),
    ));
  $rules['commerce_fraud_check_uid'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Check IP address with fraudulent order');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_hostname', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('IP address is same as one of fraudulent orders'),
    ));
  $rules['commerce_fraud_check_hostname'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score if user has placed orders from different IP addresses');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_ip', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('User have placed orders from different IP address'),
    ));
  $rules['commerce_fraud_check_ip'] = $rule;

  return $rules;
}
