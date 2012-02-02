<?php
// $Id$

/**
 * @file
 * Defines an 'Audio' Entity, used during the Wizzlern Entity training.
 *
 * @name Audio Entity
 * @author you
 */

/**
 *
 */
function audio_entity_form($form, &$form_state, $audio_entity = NULL) {
  // Create an empty entity if we're creating a new audio item instead of
  // editing an existing one.
  if (empty($audio_entity)) {
    $audio_entity = (object) array('title' => '', 'aid' => NULL);
  }

  // Store the original audio item.
  $form['#audio_entity'] = $audio_entity;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $audio_entity->title,
    '#required' => TRUE,
  );

  $form['aid'] = array(
    '#type' => 'value',
    '#value' => $audio_entity->aid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 100,
  );

  // Attach fields for this entity.
  field_attach_form('audio_entity', $audio_entity, $form, $form_state);

  return $form;
}

/**
 *
 */
function audio_entity_form_submit($form, &$form_state) {
  $values = &$form_state['values'];

  // Get the original audio item.
  $audio_entity = $form['#audio_entity'];

  // Remove unneeded values.
  form_state_values_clean($form_state);
  // Copy submitted values to entity properties.
  entity_form_submit_build_entity('audio_entity', $audio_entity, $form, $form_state);
  // Merge new values into the original entity.
  $audio_entity = (object) array_intersect_key((array) $audio_entity, $values);

  // Save the entity.
  audio_entity_save($audio_entity);

  // Clear chaches
  cache_clear_all();

  drupal_set_message(t('The changes have been saved.'));
  $form_state['redirect'] = 'admin/structure/audio_entity';
}

/**
 * Menu callback -- ask for confirmation of audio item deletion.
 */
function audio_entity_delete_confirm($form, &$form_state, $audio_entity) {
  $form['aid'] = array('#type' => 'value', '#value' => $audio_entity->aid);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array(
      '%title' => $audio_entity->title,
    )),
    'admin/structure/audio_entity',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute audio item deletion
 */
function audio_entity_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $audio_entity = audio_entity_load($form_state['values']['aid']);
    audio_entity_delete($audio_entity);
    drupal_set_message(t('%title has been deleted.', array(
      '%title' => $audio_entity->title,
    )));
  }
  $form_state['redirect'] = 'admin/structure/audio_entity';
}

/**
 * Builds the audio entities administration overview.
 */
function audio_entities_overview() {
  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'a.title'),
    'operations' => array('data' => t('Operations')),
  );
  $rows = array();

  $query = db_select('audio_entity', 'a')
    ->extend('PagerDefault')
    ->extend('TableSort');

  $aids = $query
    ->fields('a', array('aid'))
    ->limit(50)
    ->orderByHeader($header)
    ->execute()
    ->fetchCol();

  if (!empty($aids)) {
    foreach (audio_entity_load_multiple($aids) as $audio_entity) {

      $uri = entity_uri('audio_entity', $audio_entity);

      $rows[$audio_entity->aid]['title'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $audio_entity->title,
          '#href' => 'audio_entity/' . $audio_entity->aid,
        ),
      );
      $rows[$audio_entity->aid]['operations'] = array(
        'data' => array(
          '#theme' => 'links',
          '#attributes' => array('class' => array('links', 'inline')),
          '#links' => array(
            'edit' => array(
              'title' => t('edit'),
              'href' => $uri['path'] . '/edit',
            ),
            'delete' => array(
              'title' => t('delete'),
              'href' => $uri['path'] . '/delete',
            ),
          ),
        ),
      );
    }
  }
  $build['audio_entities'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No audio items available.'),
  );
  $build['pager'] = array('#markup' => theme('pager'));
  return $build;
}