import 'package:firebase_messaging/firebase_messaging.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:webview_flutter/webview_flutter.dart' show JavascriptMode, WebView, WebViewController;
import 'package:http/http.dart' as http;

Future<void> main() async {

  runApp(MyApp());
  await Firebase.initializeApp(options: DefaultFirebaseOptions.currentPlatform,);
  //await FirebaseMessaging.instance.setAutoInitEnabled(true);
}

class Application extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => _Application();
}

class _Application extends State<Application> {
  // It is assumed that all messages contain a data field with the key 'type'
  Future<void> setupInteractedMessage() async {
    // Get any messages which caused the application to open from
    // a terminated state.
    RemoteMessage? initialMessage =
        await FirebaseMessaging.instance.getInitialMessage();

    // If the message also contains a data property with a "type" of "chat",
    // navigate to a chat screen
    if (initialMessage != null) {
      _handleMessage(initialMessage);
    }

    // Also handle any interaction when the app is in the background via a
    // Stream listener
    FirebaseMessaging.onMessage.listen((RemoteMessage message) {
    print('Got a message whilst in the foreground!');
    print('Message data: ${message.data}');

    if (message.notification != null) {
      print('Message also contained a notification: ${message.notification}');
    }
  });
  }

  void _handleMessage(RemoteMessage message) {
    if (message.data['type'] == 'chat') {
      Navigator.pushNamed(context, '/chat',
        arguments: message,
      );
    }
  }

  @override
  void initState() {
    super.initState();

    // Run code required to handle interacted messages in an async function
    // as initState() must not be async
    setupInteractedMessage();
  }

  @override
  Widget build(BuildContext context) {
    return Text("...");
  }
}

class MyApp extends StatelessWidget {
  MyApp({super.key});
  late WebViewController _controller;

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Open Gate',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: Scaffold(
        appBar: AppBar(
          title: Text("Open Gate"),
        ),
        body: Center(
          child: WebView(
            initialUrl: 'http://192.168.25.42/capture',
            javascriptMode: JavascriptMode.unrestricted,
            onWebViewCreated: (WebViewController webViewController) {
              _controller = webViewController;
            },
          ),
        ),
        floatingActionButton: const FloatingActionButton.extended(
          onPressed: closeGate,
          label: Text('Open / Close'),
      ),
      ),
      );
  }
}

void closeGate() async {
  var client = http.Client();
  try
  {
    var response = await client.get(Uri.http("192.168.25.42", "/control"));
    print('Response status: ${response.statusCode}');
    print('Response body: ${response.body}');
  }
  finally
  {
    client.close();
  }
}