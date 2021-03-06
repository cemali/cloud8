<?php

/**
 * @file
 * Code required only when rendering the available updates report.
 */

use Drupal\Component\Utility\Unicode;
use Drupal\Core\Template\Attribute;
use Drupal\Core\Url;

/**
 * Prepares variables for project status report templates.
 *
 * Default template: update-report.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: An array of data about each project's status.
 */
function template_preprocess_update_report(&$variables) {
  $data = $variables['data'];

  $last = \Drupal::state()->get('update.last_check') ?: 0;

  $variables['last_checked'] = array(
    '#theme' => 'update_last_check',
    '#last' => $last,
    // Attach the library to a variable that gets printed always.
    '#attached' => array(
      'library' => array(
        'update/drupal.update.admin',
      ),
    )
  );

  // For no project update data, populate no data message.
  if (empty($data)) {
    $variables['no_updates_message'] = _update_no_data();
  }

  $rows = array();

  foreach ($data as $project) {
    $project_status = array(
      '#theme' =>  'update_project_status',
      '#project' => $project,
    );

    // Build project rows.
    if (!isset($rows[$project['project_type']])) {
      $rows[$project['project_type']] = array(
        '#type' => 'table',
        '#attributes' => array('class' => array('update')),
      );
    }
    $row_key = !empty($project['title']) ? Unicode::strtolower($project['title']) : Unicode::strtolower($project['name']);

    // Add the project status row and details.
    $rows[$project['project_type']][$row_key]['status'] = $project_status;

    // Add project status class attribute to the table row.
    switch ($project['status']) {
      case UPDATE_CURRENT:
        $rows[$project['project_type']][$row_key]['#attributes'] = array('class' => array('color-success'));
        break;
      case UPDATE_UNKNOWN:
      case UPDATE_FETCH_PENDING:
      case UPDATE_NOT_FETCHED:
      case UPDATE_NOT_SECURE:
      case UPDATE_REVOKED:
      case UPDATE_NOT_SUPPORTED:
        $rows[$project['project_type']][$row_key]['#attributes'] = array('class' => array('color-error'));
        break;
      case UPDATE_NOT_CHECKED:
      case UPDATE_NOT_CURRENT:
      default:
        $rows[$project['project_type']][$row_key]['#attributes'] = array('class' => array('color-warning'));
        break;
    }
  }

  $project_types = array(
    'core' => t('Drupal core'),
    'module' => t('Modules'),
    'theme' => t('Themes'),
    'module-disabled' => t('Disabled modules'),
    'theme-disabled' => t('Disabled themes'),
  );

  $variables['project_types'] = array();
  foreach ($project_types as $type_name => $type_label) {
    if (!empty($rows[$type_name])) {
      ksort($rows[$type_name]);
      $variables['project_types'][] = array(
        'label' => $type_label,
        'table' => $rows[$type_name],
      );
    }
  }
}

/**
 * Prepares variables for update project status templates.
 *
 * Default template: update-project-status.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - project: An array of information about the project.
 */
function template_preprocess_update_project_status(&$variables) {
  // Storing by reference because we are sorting the project values.
  $project = &$variables['project'];

  // Set the project title and URL.
  $variables['title'] = (isset($project['title'])) ? $project['title'] : $project['name'];
  $variables['url'] = (isset($project['link'])) ? Url::fromUri($project['link'])->toString() : NULL;

  $variables['install_type'] = $project['install_type'];
  if ($project['install_type'] == 'dev' && !empty($project['datestamp'])) {
    $variables['datestamp'] = format_date($project['datestamp'], 'custom', 'Y-M-d');
  }

  $variables['existing_version'] = $project['existing_version'];

  $versions_inner = array();
  $security_class = array();
  $version_class = array();
  if (isset($project['recommended'])) {
    if ($project['status'] != UPDATE_CURRENT || $project['existing_version'] !== $project['recommended']) {

      // First, figure out what to recommend.
      // If there's only 1 security update and it has the same version we're
      // recommending, give it the same CSS class as if it was recommended,
      // but don't print out a separate "Recommended" line for this project.
      if (!empty($project['security updates'])
        && count($project['security updates']) == 1
        && $project['security updates'][0]['version'] === $project['recommended']
      ) {
        $security_class[] = 'project-update__version--recommended';
        $security_class[] = 'project-update__version---strong';
      }
      else {
        $version_class[] = 'project-update__version--recommended';
        // Apply an extra class if we're displaying both a recommended
        // version and anything else for an extra visual hint.
        if ($project['recommended'] !== $project['latest_version']
          || !empty($project['also'])
          || ($project['install_type'] == 'dev'
            && isset($project['dev_version'])
            && $project['latest_version'] !== $project['dev_version']
            && $project['recommended'] !== $project['dev_version'])
          || (isset($project['security updates'][0])
            && $project['recommended'] !== $project['security updates'][0])
        ) {
          $version_class[] = 'project-update__version--recommended-strong';
        }
        $versions_inner[] = array(
          '#theme' => 'update_version',
          '#version' => $project['releases'][$project['recommended']],
          '#title' => t('Recommended version:'),
          '#attributes' => array('class' => $version_class),
        );
      }

      // Now, print any security updates.
      if (!empty($project['security updates'])) {
        $security_class[] = 'version-security';
        foreach ($project['security updates'] as $security_update) {
          $versions_inner[] = array(
            '#theme' => 'update_version',
            '#version' => $security_update,
            '#title' => t('Security update:'),
            '#attributes' => array('class' => $security_class),
          );
        }
      }
    }

    if ($project['recommended'] !== $project['latest_version']) {
      $versions_inner[] = array(
        '#theme' => 'update_version',
        '#version' => $project['releases'][$project['latest_version']],
        '#title' => t('Latest version:'),
        '#attributes' => array('class' => array('version-latest')),
      );
    }
    if ($project['install_type'] == 'dev'
      && $project['status'] != UPDATE_CURRENT
      && isset($project['dev_version'])
      && $project['recommended'] !== $project['dev_version']) {
      $versions_inner[] = array(
        '#theme' => 'update_version',
        '#version' => $project['releases'][$project['dev_version']],
        '#title' => t('Development version:'),
        '#attributes' => array('class' => array('version-latest')),
      );
    }
  }

  if (isset($project['also'])) {
    foreach ($project['also'] as $also) {
      $versions_inner[] = array(
        '#theme' => 'update_version',
        '#version' => $project['releases'][$also],
        '#title' => t('Also available:'),
        '#attributes' => array('class' => array('version-also-available')),
      );
    }
  }

  if (!empty($versions_inner)) {
    $variables['versions'] = $versions_inner;
  }

  if (!empty($project['disabled'])) {
    sort($project['disabled']);
    $variables['disabled'] = $project['disabled'];
  }

  sort($project['includes']);
  $variables['includes'] = $project['includes'];

  $variables['extras'] = array();
  if (!empty($project['extra'])) {
    foreach ($project['extra'] as $value) {
      $extra_item = array();
      $extra_item['attributes'] = new Attribute();
      $extra_item['label'] = $value['label'];
      $extra_item['data'] = [
        '#prefix' => '<em>',
        '#markup' => $value['data'],
        '#suffix' => '</em>'
      ];
      $variables['extras'][] = $extra_item;
    }
  }

  // Set the project status details.
  $status_label = NULL;
  switch ($project['status']) {
    case UPDATE_NOT_SECURE:
      $status_label = t('Security update required!');
      break;
    case UPDATE_REVOKED:
      $status_label = t('Revoked!');
      break;
    case UPDATE_NOT_SUPPORTED:
      $status_label = t('Not supported!');
      break;
    case UPDATE_NOT_CURRENT:
      $status_label = t('Update available');
      break;
    case UPDATE_CURRENT:
      $status_label = t('Up to date');
      break;
  }
  $variables['status']['label'] = $status_label;
  $variables['status']['attributes'] = new Attribute();
  $variables['status']['reason'] = (isset($project['reason'])) ? $project['reason'] : NULL;

  switch ($project['status']) {
    case UPDATE_CURRENT:
      $uri = 'core/misc/icons/73b355/check.svg';
      $text = t('Ok');
      break;
    case UPDATE_UNKNOWN:
    case UPDATE_FETCH_PENDING:
    case UPDATE_NOT_FETCHED:
      $uri = 'core/misc/icons/e29700/warning.svg';
      $text = t('Warning');
      break;
    case UPDATE_NOT_SECURE:
    case UPDATE_REVOKED:
    case UPDATE_NOT_SUPPORTED:
      $uri = 'core/misc/icons/e32700/error.svg';
      $text = t('Error');
      break;
    case UPDATE_NOT_CHECKED:
    case UPDATE_NOT_CURRENT:
    default:
      $uri = 'core/misc/icons/e29700/warning.svg';
      $text = t('Warning');
      break;
  }

  $variables['status']['icon'] = array(
    '#theme' => 'image',
    '#width' => 18,
    '#height' => 18,
    '#uri' => $uri,
    '#alt' => $text,
    '#title' => $text,
  );
}
