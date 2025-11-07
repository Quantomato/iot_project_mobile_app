import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:iot_project_mobile_app/firebase_options.dart';
import 'package:iot_project_mobile_app/utils/snackbar_utils.dart';
import 'package:iot_project_mobile_app/utils/nfc_reader_utils.dart';


void main() async{
  WidgetsFlutterBinding.ensureInitialized();
  await SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
    DeviceOrientation.portraitDown,
  ]);
  await Firebase.initializeApp(options: DefaultFirebaseOptions.currentPlatform,);
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});
  
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'BollardBros',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.red),
      ),
      home: const LoginPage(title: 'BollardBros Login Page'),
    );
  }
}

//Create LoginPage------------------------------------------------------------------------------------------------------------
class LoginPage extends StatefulWidget {
  const LoginPage({super.key, required this.title});

  final String title;

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  final TextEditingController _textControllerEmail = TextEditingController();
  final TextEditingController _textControllerPassword = TextEditingController();

  Future<UserCredential?> canUserSignin(String email, String password) async{
    try{
      final credential = await FirebaseAuth.instance.signInWithEmailAndPassword(email: email, password: password);
      print("Logged in Successfully");
      return credential;
    }on FirebaseAuthException catch(e){
      if(!mounted){return null;}
      if (e.code == 'user-not-found') {
        print("Email does not exist.");
        SnackBarUtils.showErrorSnackBar(context, "User with inputted email not found!");
      } else if (e.code == 'wrong-password') {
        print("Incorrect password.");
        SnackBarUtils.showErrorSnackBar(context, "Password incorrect!");
      }
      return null;
    }
  }

  @override
  void dispose(){
    _textControllerEmail.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            //Bollardbros text
            SizedBox(
              width: 260,
              child: Text(
                "BollardBros Login",
                textAlign: TextAlign.center,
                style: TextStyle(
                  color: Colors.black,
                  fontSize: 31.0,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),
            const SizedBox(height: 25),

            // Email TextField
            SizedBox(
              width: 250,
              child: TextField(
                controller: _textControllerEmail,
                decoration: const InputDecoration(
                  labelText: 'Email',
                  border: OutlineInputBorder(),
                ),
              ),
            ),
            const SizedBox(height: 16),

            // Password TextField
            SizedBox(
              width: 250,
              child: TextField(
                controller: _textControllerPassword,
                obscureText: true,
                decoration: const InputDecoration(
                  labelText: 'Password',
                  border: OutlineInputBorder(),
                ),
              ),
            ),
            const SizedBox(height: 50),

            // Login button
            SizedBox(
              width: 200,
              height: 50,
              child: ElevatedButton(
                onPressed: () async{
                  String email = _textControllerEmail.text;
                  String password = _textControllerPassword.text;
                  final user = await canUserSignin(email, password);
                  if(user != null){
                    Navigator.pushReplacement(
                      context,
                      MaterialPageRoute(builder: (context) => MainPage(title: "Main Page", user: user,))
                    );
                  }
                },
                child: const Text(
                  'Login',
                  style: TextStyle(fontSize: 18),
                ),
              ),
            ),
            const SizedBox(height: 30),
            //Register Button
            SizedBox(
              width: 200,
              height: 50,
              child: ElevatedButton(
                onPressed: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => const RegisterPage(title: "Register Page")),
                  );
                },
                child: const Text(
                  'Register',
                  style: TextStyle(fontSize: 18),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

//Create RegisterPage--------------------------------------------------------------------------------------------------------
class RegisterPage extends StatefulWidget {
  const RegisterPage({super.key, required this.title});

  final String title;

  @override
  State<RegisterPage> createState() => _RegisterPageState();
}

class _RegisterPageState extends State<RegisterPage>{
  final TextEditingController _textControllerEmail = TextEditingController();
  final TextEditingController _textControllerPassword = TextEditingController();
  final TextEditingController _textControllerConfirmPassword = TextEditingController();

  //Function to check if user can register
  Future<bool> canUserRegister(String email, String password) async {
    try {
      final auth = FirebaseAuth.instance;
      // Get a list of sign-in methods for the provided email
      final signInMethods =
          await auth.createUserWithEmailAndPassword(email: email, password: password,);

      //Registration Succeeded
      return true;
    } on FirebaseAuthException catch (e) {
      // Handle invalid email formats gracefully
      if(!mounted){return false;}
      if (e.code == 'email-already-in-use') {
        SnackBarUtils.showErrorSnackBar(context, 'Email is already in use!');
        return false;
      }else if(e.code == 'invalid-email'){
        SnackBarUtils.showErrorSnackBar(context, 'Invalid Email Format');
        return false;
      }else if(e.code == 'weak-password'){
        SnackBarUtils.showErrorSnackBar(context, 'Password is too weak!');
        return false;
      }else{
        SnackBarUtils.showErrorSnackBar(context, 'Registration Failed: ${e.message}');
        return false;
      }
    }catch (e) {
      //When I catch you
      return false;
    }
  }

  @override
  void dispose(){
    _textControllerEmail.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context){
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            //Bollardbros text
            SizedBox(
              width: 260,
              child: Text(
                "Register an Account",
                textAlign: TextAlign.center,
                style: TextStyle(
                  color: Colors.black,
                  fontSize: 27.0,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),
            const SizedBox(height: 25),

            // Email TextField
            SizedBox(
              width: 250,
              child: TextField(
                controller: _textControllerEmail,
                decoration: const InputDecoration(
                  labelText: 'Email',
                  border: OutlineInputBorder(),
                ),
              ),
            ),
            const SizedBox(height: 16),

            // Password TextField
            SizedBox(
              width: 250,
              child: TextField(
                controller: _textControllerPassword,
                obscureText: true,
                decoration: const InputDecoration(
                  labelText: 'Password',
                  border: OutlineInputBorder(),
                ),
              ),
            ),
            const SizedBox(height: 16),

            // Confirm Password TextField
            SizedBox(
              width: 250,
              child: TextField(
                controller: _textControllerConfirmPassword,
                obscureText: true,
                decoration: const InputDecoration(
                  labelText: 'Confirm Password',
                  border: OutlineInputBorder(),
                ),
              ),
            ),
            const SizedBox(height: 50),

            // Register button
            SizedBox(
              width: 200,
              height: 50,
              child: ElevatedButton(
                onPressed: () async{
                  String email = _textControllerEmail.text.trim();
                  String password = _textControllerPassword.text.trim();
                  String confirmPassword = _textControllerConfirmPassword.text.trim();

                  if (email.isEmpty) {
                    SnackBarUtils.showErrorSnackBar(context, 'Email field is empty!');
                  } else if (password.isEmpty || confirmPassword.isEmpty) {
                    SnackBarUtils.showErrorSnackBar(context, 'Password or Confirm Password field is empty!');
                  } else if (password != confirmPassword) {
                    SnackBarUtils.showErrorSnackBar(context, 'Passwords do not match!');
                  } else if (password.length < 8){
                    SnackBarUtils.showErrorSnackBar(context, 'Password is too short, must be at least 8 characters long!');
                  } else {
                    //Register user logic here
                    bool canRegister = await canUserRegister(email, password);
                    if(canRegister){
                      //Display snackbar to say user registered!
                      final messenger = ScaffoldMessenger.of(context);
                      messenger.showSnackBar(
                        SnackBar(
                          content: Text(
                            'User Successfully Registered!',
                            style: const TextStyle(color: Colors.white),
                          ),
                          backgroundColor: Colors.green,
                          duration: const Duration(seconds: 5),
                          behavior: SnackBarBehavior.floating,
                        ),
                      ).closed.then((_) {
                      });
                      Navigator.pop(context);
                    }
                  }
                },
                child: const Text(
                  'Register',
                  style: TextStyle(fontSize: 18),
                ),
              ),
            ),
            const SizedBox(height: 30),

            //Back Button
            SizedBox(
              width: 70,
              height: 50,
              child: ElevatedButton(
                onPressed: () {
                  ScaffoldMessenger.of(context).hideCurrentSnackBar();
                  Navigator.pop(context);
                },
                child: const Icon(Icons.arrow_back),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
//----------------------------------------------------------------------------------------------------------------------------------

//MainPage Widgets-----------------------------------------------------------------------------------------------------------------
class MainPage extends StatefulWidget {
  const MainPage({super.key, required this.title, required this.user});

  final String title;
  final UserCredential? user;

  @override
  State<MainPage> createState() => _MainPageState();
}

class _MainPageState extends State<MainPage>{
  bool _parkingSpaceTaken = false;
  int _parkingSpaceIndex = 0;
  bool _bollardRaised = false;

  void _removeParkingSpace() async {
    //Get Database jawn
    final userCredential = widget.user;
    final currentUser = userCredential?.user;
    if (currentUser == null) return;
    try {
      final querySnapshot = await FirebaseFirestore.instance
          .collection('parkingSpaces')
          .where('userID', isEqualTo: currentUser.uid)
          .get();

      if (querySnapshot.docs.isNotEmpty) {
        // User has a parking space
        final doc = querySnapshot.docs.first;
        final data = doc.data();
        DocumentReference parkingSpotRef = doc.reference;
        //Update Database First
        await parkingSpotRef.update({'userID': ""});
        SnackBarUtils.showSuccessSnackBar(context, 'Successfully surrendered parking spot #$_parkingSpaceIndex!');
        setState((){
          _parkingSpaceIndex = 0;
          _parkingSpaceTaken = false;
        });
      } else {
        // User does not have a parking space (should be impossible)
        SnackBarUtils.showErrorSnackBar(context, 'How on earth did you pull this off?');
      }
    } catch (e) {}
  }

  Future<void> _checkUserParkingSpace() async {
    final userCredential = widget.user;
    final currentUser = userCredential?.user;

    if (currentUser == null) return;

    try {
      final querySnapshot = await FirebaseFirestore.instance
          .collection('parkingSpaces')
          .where('userID', isEqualTo: currentUser.uid)
          .get();

      if (querySnapshot.docs.isNotEmpty) {
        // User has a parking space
        final doc = querySnapshot.docs.first;
        final data = doc.data();
        setState(() {
          _parkingSpaceIndex = data['spaceIndex'] ?? 0;
          _parkingSpaceTaken = true;
        });
      } else {
        // User does not have a parking space
        setState(() {
          _parkingSpaceIndex = 0;
          _parkingSpaceTaken = false;
        });
      }
    } catch (e) {
      print('Error checking user parking space: $e');
      setState(() {
        _parkingSpaceTaken = false;
        _parkingSpaceIndex = 0;
      });
    }
  }

  @override
  void initState() {
    super.initState();
    _checkUserParkingSpace();
  }

  @override
  Widget build(BuildContext context){
    final userCredential = widget.user;
    final email = userCredential?.user?.email ?? 'No email found';

    return Scaffold(
      body: Stack(
        children: <Widget>[
          //Top Textbox (Displays user email)
          Align(
            alignment: Alignment.topCenter,
            child: Padding(
              padding: const EdgeInsets.only(top: 80.0),
              child: Container(
                padding: const EdgeInsets.only(right:0),
                child: Text('Welcome $email', textAlign: TextAlign.center, style: TextStyle(fontSize: 20)),
              ),
            ),
          ),

          //Logout button
          Align(
            alignment: Alignment.topRight,
            child: Padding(
              padding: const EdgeInsets.only(top: 60.0),
              child: SizedBox(
                width: 120,
                height: 50,
                child: ElevatedButton(
                  onPressed: () {
                    Navigator.pushReplacement(
                        context,
                        MaterialPageRoute(builder: (context) => LoginPage(title: "Login Page",))
                      );
                  },
                  child: const Text("Logout"),
                ),
              ),
            ),
          ),

          //Parking Space Reserved or not
          Align(
            alignment: Alignment.center,
            child: Container(
              padding: const EdgeInsets.all(12),
              color: _parkingSpaceTaken ? Colors.green : Colors.red,
              child: Text(
                _parkingSpaceTaken ? 'Parking Space #$_parkingSpaceIndex taken' : 'No Parking Space Taken',
                style: TextStyle(fontSize: 20)
              ),
            ),
          ),

          //Scan RFID Button OR Display Raise/Lower Bollard Switch
          Align(
            alignment: Alignment.bottomCenter,
            child: Padding(
              padding: const EdgeInsets.only(bottom: 30.0),
              child: _parkingSpaceTaken ?
              Column(
                mainAxisSize: MainAxisSize.min,
                children: <Widget>[
                  //When user has parking space
                  Row(
                    mainAxisSize: MainAxisSize.min,
                    children: <Widget>[
                      const Text(
                        'Raise/Lower Bollard:', 
                        style: TextStyle(fontSize: 18),
                      ),
                      const SizedBox(width: 10),
                    //Switch to Raise/Lower Bollard
                      Switch(
                        value: _bollardRaised,
                        onChanged: (newValue) {
                          setState(() {
                            _bollardRaised = newValue;
                          });
                        },
                      ),
                    ],
                  ),
                  const SizedBox(height: 30),
                  Text(
                    "Bollard Currently: ${_bollardRaised ? "Raised" : "Lowered"}",
                    style: TextStyle(fontSize: 24),
                  ),
                  const SizedBox(height: 30),
                  ElevatedButton(
                    onPressed: () {
                      _bollardRaised ? SnackBarUtils.showErrorSnackBar(context, "Please lower your bollard before surrendering your parking spot.") : _removeParkingSpace();
                    },
                    child: const Text('Surrender Parking Space'),
                  ),
                  const SizedBox(height:150),
                ],
              )
              //Show the scan RFID button if user does not have parking space
              : Column(
                mainAxisSize: MainAxisSize.min,
                children: <Widget>[
                  ElevatedButton(
                  onPressed: () async {
                    await _checkUserParkingSpace();
                    Navigator.push(
                      context,
                      MaterialPageRoute(builder: (context) => NfcReaderPage(user: userCredential,)),
                    ).then((_){
                      _checkUserParkingSpace();
                    });
                  },
                  child: const Text('Scan RFID'),
                  ),
                  const SizedBox(height:150),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  } 
}