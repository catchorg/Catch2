import 'package:flutter/material.dart';
import 'package:flutter_workshop/screens/about_screen.dart';
import 'package:flutter_workshop/screens/home_demo_screen.dart';
import 'package:flutter_workshop/screens/home_screen.dart';
import 'package:flutter_workshop/screens/product_detail_screen.dart';
import 'package:flutter_workshop/screens/product_screen.dart';
import 'package:flutter_workshop/screens/random_words_screen.dart';
import 'package:flutter_workshop/screens/unknown_screen.dart';
import 'package:device_simulator/device_simulator.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        initialRoute: '/',
        routes: {
          HomeScreen.routeName: (_) => DeviceSimulator(
              brightness: Brightness.dark, enable: true, child: HomeScreen()),
          ProductScreen.routeName: (_) => ProductScreen(),
          ProductDetailScreen.routeName: (_) => ProductDetailScreen(),
          RandomWordsScreen.routeName: (_) => RandomWordsScreen(),
          HomeDemoScreen.routeName: (_) => HomeDemoScreen(),
          AboutScreen.routeName: (_) => AboutScreen()
        },
        onUnknownRoute: (_) =>
            MaterialPageRoute(builder: (_) => UnknownScreen()));
  }
}

// From https://github.com/Jadhielv/flutter-workshop/blob/master/lib/main.dart