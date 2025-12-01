import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter_nfc_kit/flutter_nfc_kit.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:cloud_firestore/cloud_firestore.dart';


class NfcReaderPage extends StatefulWidget {
  const NfcReaderPage({super.key, required this.user});
  final UserCredential? user;

  @override
  State<NfcReaderPage> createState() => _NfcReaderPageState();
}

class _NfcReaderPageState extends State<NfcReaderPage> {
  String _status = 'Idle';
  String? _tagId;
  String result = 'started';
  int parkingSpotID = 0;
  bool _queryingNfc = false;

  //Result helper
  String _getMessageForResult(String result) {
    if (result.startsWith('success')) {
      return 'Parking Spot #$parkingSpotID successfully registered!';
    } else if (result.startsWith('error')) {
      return 'An error occurred while reading the tag. Please try again.';
    } else if(result.startsWith('spot occupied')){
      return 'Parking spot #$parkingSpotID already occupied, please try another spot.';
    } else if(result.startsWith('invalid nfc')){
      return 'This parking spot does not exist, please try again or contact assistance.';
    } else if(result.startsWith('car not parked')){
      return 'You have not parked in parking spot #$parkingSpotID.';
    }else if(result.startsWith('started')){
      return 'Press \'Scan NFC Tag\' button to begin scanning.';
    } else {
      return 'Unknown result: $result. Please try again.';
    }
  }

  //Parking Spot Verifyer Helper
  Future<bool> _checkParkingSpot(String nfcId) async {
    try {
      //Verify Authentication
      final currentUser = FirebaseAuth.instance.currentUser;
      if (currentUser == null) {
        result = 'error: not signed in';
        return false;
      }
      // Reference to the "parkingSpaces" collection
      final querySnapshot = await FirebaseFirestore.instance
          .collection('parkingSpaces')
          .where('nfcID', isEqualTo: nfcId)
          .get();

      if (querySnapshot.docs.isNotEmpty) {
        // Found a matching document, now check if parking spot is valid
        var doc = querySnapshot.docs.first;
        Map<String, dynamic> data = doc.data();
        bool isUserParked = data['carIsParked'];
        String userID = data['userID'] as String;
        parkingSpotID = data['spaceIndex'];
        if(userID != ""){
          result = 'spot occupied';
          return false;
        }else if(!isUserParked){
          result = 'car not parked';
          return false;
        }else{
          //User takes over spot with their user ID
          DocumentReference parkingSpotRef = doc.reference;
          await parkingSpotRef.update({
            'userID': currentUser.uid,
          });
          result = 'success';
        }
        return true;
      } else {
        result = 'invalid nfc';
        return false;
      }
    } catch (e) {
      result += e.toString();
      return false;
    }
  }

  //NFC Session Manager
  Future<String> _startNfcSession() async {
    if (!mounted) return 'error';
    setState(() {
      _status = 'Starting NFC session...';
      _tagId = null;
      _queryingNfc = true;
    });

    try {
      // Poll for an NFC tag for up to 20 seconds
      NFCTag tag = await FlutterNfcKit.poll(timeout: Duration(seconds: 15));

      if (!mounted) return 'error';

      // If no tag is returned, it likely timed out
      if (tag.id == null || tag.id!.isEmpty) {
        if(mounted){
          setState(() {
            _status = 'Program timed out';
            _queryingNfc = false;
          });
        }
        
        return 'error';
      }

      // Tag successfully read
      String id = tag.id!;
      bool spotValid = await _checkParkingSpot(id);
      if(mounted){
        setState(() {
          _status = 'Tag read successfully!';
          _tagId = id;
          _queryingNfc = false;
        });
      }
      
      //Call message
      return 'success';
    } on TimeoutException {
      if(mounted){
        setState(() {
          _status = 'NFC session timed out.';
          _queryingNfc = false;
        });
      }
      
      return 'error';
    } on Exception catch (e) {
      if(mounted){
        setState(() {
          _status = 'NFC error: $e';
          _queryingNfc = false;
        });
      }
      
      return 'error';
    } finally {
      // Safely end the NFC session
      try {
        await FlutterNfcKit.finish();
      } catch (_) {}
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('NFC Tag Reader')),
      body: Stack(
        children: <Widget>[
          //Top Icon
          if(_queryingNfc)
            Align(
              alignment:Alignment.topCenter,
              child: Icon(Icons.contactless_rounded, size:300.0, color: Colors.blue),
            ),
          //Text Determining Status
          Align(
            alignment: Alignment.center,
            child: Padding(
              padding: const EdgeInsets.only(top: 50.0),
              child: Container(
                padding: const EdgeInsets.only(right:0),
                child: Text(_queryingNfc ? 'Hold phone up to parking spot card' : _getMessageForResult(result), textAlign: TextAlign.center, style: TextStyle(fontSize: 20)),
              ),
            ),
          ),
          //Button to Scan NFC Tag
          if(!_queryingNfc)
            Align(
              alignment: Alignment.bottomCenter,
              child: Padding(
              padding: const EdgeInsets.only(bottom:60.0),
                child: ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.deepPurpleAccent,
                    foregroundColor: Colors.white,
                  ),
                  onPressed: () async{
                    await _startNfcSession();
                    // Check if successful and navigate back
                    if (result == 'success') {
                      Navigator.pop(context);
                    }
                  },
                  child: Text('Scan NFC Tag'),
                ),
              ),
            ),
        ], 
      ),
    );
  }
}
