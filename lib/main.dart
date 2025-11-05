import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';

void main() {
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
                onPressed: () {
                  String email = _textControllerEmail.text;
                  String password = _textControllerPassword.text;
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
    @override
    Widget build(BuildContext context){
      return Scaffold(

      );
    }
}

