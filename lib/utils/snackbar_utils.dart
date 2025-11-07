import 'package:flutter/material.dart';

class SnackBarUtils {
  // Static variable to track if a SnackBar is currently showing.
  // This state is shared across all calls to the static method.
  static bool _isSnackBarActive = false;

  static void showErrorSnackBar(BuildContext context, String message) {
    // Prevent showing a new SnackBar if one is already active.
    if (_isSnackBarActive) return;

    // Set the flag to active before showing the SnackBar
    _isSnackBarActive = true;
    
    // Get the ScaffoldMessenger
    final messenger = ScaffoldMessenger.of(context);

    // Show the SnackBar and use its .closed future to reset the flag.
    messenger.showSnackBar(
      SnackBar(
        content: Text(
          message,
          style: const TextStyle(color: Colors.white),
        ),
        backgroundColor: Colors.red,
        duration: const Duration(seconds: 3),
        behavior: SnackBarBehavior.floating,
      ),
    ).closed.then((reason) {
      // Reset flag when the SnackBar is dismissed
      _isSnackBarActive = false;
    });
  }

  static void showSuccessSnackBar(BuildContext context, String message) {
    // Prevent showing a new SnackBar if one is already active.
    if (_isSnackBarActive) return;

    // Set the flag to active before showing the SnackBar
    _isSnackBarActive = true;
    
    // Get the ScaffoldMessenger
    final messenger = ScaffoldMessenger.of(context);

    // Show the SnackBar and use its .closed future to reset the flag.
    messenger.showSnackBar(
      SnackBar(
        content: Text(
          message,
          style: const TextStyle(color: Colors.white),
        ),
        backgroundColor: Colors.green,
        duration: const Duration(seconds: 3),
        behavior: SnackBarBehavior.floating,
      ),
    ).closed.then((reason) {
      // Reset flag when the SnackBar is dismissed
      _isSnackBarActive = false;
    });
  }
}