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
      'order_status' => 'commerce_order_status_options_list',
    ));
  $rules['commerce_fraud_update_order_status'] = $rule;
  if (module_exists('simple_mail')) {
    $rule = rules_reaction_rule();
    $rule->label = t('Email black listed orders');
    $rule->tags = array('Commerce Fraud');
    $rule->active = FALSE;
    $rule
      ->event('commerce_checkout_complete')
      ->condition('commerce_fraud_blacklisted', array(
        'commerce_order:select' => 'commerce_order',
        'commerce_fraud_level_function' => 'commerce_fraud_is_blacklisted',
      ))
      ->action('email', array(
        'commerce_order:select' => 'commerce_order',
      ));
    $rules['commerce_fraud_email_blacklist'] = $rule;
  }

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
      'commerce_fraud_po_box_function' => 'commerce_fraud_order_has_pobox',
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
  $rules['commerce_fraud_anonymous_user'] = $rule;
  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score if user is anonymous');
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
      'description' => t('Order User anonymous'),
    ));
  $rules['commerce_fraud_anonymous_user'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Increase fraud score for orders placed within x minutes');
  $rule->tags = array('Commerce Fraud');
  $rule->active = FALSE;
  $rule
    ->event('commerce_checkout_complete')
    ->condition('commerce_fraud_last_order', array(
      'commerce_order:select' => 'commerce-order',
    ))
    ->action('commerce_fraud_increase_fraud_count', array(
      'commerce_order:select' => 'commerce_order',
      'counter' => 5,
      'description' => t('Previous order completed within 5 minutes'),
    ));
  $rules['commerce_fraud_last_order'] = $rule;

  return $rules;
}
