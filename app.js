'use strict'
let connection;
// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { json } = require('express');
const { stringify } = require('querystring');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    // CHANGE THIS

    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
  
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 


let sharedLib = ffi.Library('./libgpxparser', {
    'gpxJSON': [ 'string', [ 'string' ] ],
    'validateGPXJSON': [ 'int', ['string','string'] ],
    'routeListJSON': ['string', ['string'] ],
    'trackListJSON': ['string', ['string'] ],
    'renameComponent': ['int', ['string','string','string']],
    'addJSONroute': ['int', ['string','string']],
    'addJSONwaypoint': ['int',['string','string']],
    'jsonGPX':['int', ['string','string']],
    'renameProper':['int', ['string','string','string','int']],
    'waypointListJSON':['string', ['string','string']]
});

// let gpxDoc = JSON.parse(gpxJSONstr);

app.get('/gpxAttributes', function (req,res) {
  let files = fs.readdirSync("uploads");
  let JSONObjectArray = [];

  files.forEach((file) => {
    let filePath = 'uploads/' + file;

    let JSONparsed;
    let JSONstring = sharedLib.gpxJSON(filePath);
    // console.log(JSONstring +"\n");
    let jsonBool = sharedLib.validateGPXJSON(filePath,'parser/src/gpx.xsd');
    if (JSONstring !== "{}" && jsonBool === 1 ) {
      JSONparsed = JSON.parse(JSONstring);
      JSONparsed.fileName = file;
      // console.log("name: "+JSONparsed.fileName);
      // console.log("version: "+JSONparsed.version);
		  // console.log("creator: "+JSONparsed.creator);
      // console.log("wpt: "+JSONparsed.numWaypoints);
		  // console.log("rte: "+JSONparsed.numRoutes);
      // console.log("trk: "+JSONparsed.numTracks);
      JSONObjectArray.push(JSONparsed);
    } else {
      fs.unlinkSync(filePath)
    }
  });
  res.send(JSONObjectArray);
});

let componentFileName = "";

//not using anymore
app.get('/componentPanel', function (req,res) {
  let files = fs.readdirSync("uploads");
  let JSONObjectArray = [];

  // console.log("comp\n");

  files.forEach((file) => {
    let filePath = 'uploads/' + file;
    // console.log(filePath);
    // let routeParsed;
    let trackListParsed;
    let routeListParsed;
    let jsonBool = sharedLib.validateGPXJSON(filePath,'parser/src/gpx.xsd');
    // console.log("validate: " + jsonBool);
    let JSONroutestring = sharedLib.routeListJSON(filePath);
    let JSONtrackstring = sharedLib.trackListJSON(filePath)
    // console.log("route: " + JSONroutestring +"\n");
    // console.log("track: " + JSONtrackstring +"\n");

  });
  res.send(JSONObjectArray);
});

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.stuff + " " + req.query.junk;
  res.send({
    stuff: retStr
  });
});

app.get('/renameEndpoint', async function(req , res){

  let filePath = 'uploads/' + req.query.fileName;
  let newName = req.query.newName;
  let oldName = req.query.oldName;
  console.log(filePath);
  console.log(newName);
  console.log(oldName);

  let renameBool = sharedLib.renameComponent(filePath,newName,oldName);

  // console.log("rename: " + renameBool);

  res.send({
    stuff: renameBool
  });
});

app.get('/renameProperEndpoint', function(req , res){

  let filePath = 'uploads/' + req.query.fileName;
  let newName = req.query.newName;
  let oldName = req.query.oldName;
  // console.log(filePath);
  // console.log(newName);
  // console.log(oldName);

  let compNum = 0;
  let compName = "";

  let renameBool = sharedLib.renameProper(filePath,newName,oldName);

  // console.log("rename: " + renameBool);

  res.send({
    stuff: filePath
  });
});

app.get('/addRouteEndpoint', function(req , res){

  let rt = req.query.rt;
  let waypointArray = req.query.waypoints;
  let filePath = 'uploads/' + req.query.fileName;

  //add route\
  let routeStr = JSON.stringify(rt);
  // console.log(rt);
  // console.log(routeStr);

  // console.log(waypointArray);

  // console.log(routeStr);

  let routeBool = sharedLib.addJSONroute(filePath, routeStr);

  waypointArray.forEach(function(wpt){
    // console.log(wpt);
    // console.log(wpt.lat);
    wpt.lat = parseFloat(wpt.lat);
    wpt.lon = parseFloat(wpt.lon);
    let wptStr = JSON.stringify(wpt);
    // console.log(wptStr);
    let wptBool = sharedLib.addJSONwaypoint(filePath, wptStr);

  });

  res.send({
    stuff: routeStr
  });
});

app.get('/createGPXEndpoint', function(req , res){
  let filePath = 'uploads/' + req.query.fileName; 
  let docCreator = req.query.creator;
  let docVersion = req.query.version;

  let gpxData = {
    version: 1.1,
    creator: docCreator
  };

  let jsonDocString = JSON.stringify(gpxData);

  console.log(jsonDocString);
  console.log(filePath);

  let createGPX = sharedLib.jsonGPX(jsonDocString, filePath);

  // console.log()

  // res.send({
  //   stuff: createGPX
  // });
});

app.get('/componentQuery', function(req, res){
  let retStr = req.query.fileName;
  let filePath = 'uploads/' + retStr;

  console.log(filePath);

  let JSONObjectArray = [];

  // let routeParsed;
  let trackListParsed;
  let routeListParsed;
  let jsonBool = sharedLib.validateGPXJSON(filePath,'parser/src/gpx.xsd');
  // console.log("validate: " + jsonBool);
  let JSONroutestring = sharedLib.routeListJSON(filePath);
  let JSONtrackstring = sharedLib.trackListJSON(filePath);
  // console.log("route: " + JSONroutestring +"\n");
  // console.log("track: " + JSONtrackstring +"\n");
  

  let routeNum = 1;

  if (JSONroutestring !== "[]" && jsonBool === 1 ) {

    routeListParsed = JSON.parse(JSONroutestring.replace("\n","\\n"));

    routeListParsed.forEach(function(routeParsed){
      routeParsed.fileName = retStr;
      routeParsed.compName = "Route " + routeNum++;
      // console.log("compname: "+routeParsed.compName);
      // console.log("name: "+routeParsed.name);
      // console.log("numPoints: "+routeParsed.numPoints);
      // console.log("length: "+routeParsed.len);
      // console.log("loop: "+routeParsed.loop);
      JSONObjectArray.push(routeParsed);
    }) 
          
  }

  let trackNum = 1;
  if (JSONtrackstring !== "[]" && jsonBool === 1) {

    trackListParsed = JSON.parse(JSONtrackstring.replace("\n","\\n"));

    trackListParsed.forEach(function(trackParsed){
      trackParsed.fileName = retStr;
      trackParsed.compName = "Track " + trackNum++;
      JSONObjectArray.push(trackParsed);
    }) 
  }

  
  
  res.send(JSONObjectArray);
});

app.get('/otherData', function(req,res){

  let fname = req.query.fileName;
  
  let filePath = 'uploads/' + fname;
  let otherDataJSON = sharedLib.routeListJSON(filePath);
  let trackDataJSON = sharedLib.trackListJSON(filePath);

  let routeParsed = JSON.parse(otherDataJSON.replace("\n", "\\n"));
  let trackParsed = JSON.parse(trackDataJSON.replace("\n","\\n"));

  let dataParsed = [];
  dataParsed.push(routeParsed);
  dataParsed.push(trackParsed);
  
  res.send(dataParsed);
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

let dbConf = {};

app.get('/clearDataEndpoint', async function(req,res){
  // dbConf = {
  //   host     : 'dursley.socs.uoguelph.ca',
  //   user     : req.query.username,
  //   password : req.query.password,
  //   database : req.query.database
  // };

  let fileCount = 0;
  let routeCount = 0;
  let pointCount = 0;

  const mysql = require('mysql2/promise');
    
  let connection;

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf);

      connection.execute("DELETE FROM POINT;");
      connection.execute("DELETE FROM ROUTE;");
      connection.execute("DELETE FROM FILE;");
      
      

      const [files, fields1] = await connection.execute("SELECT * FROM `FILE`");
      const [routes, fields2] = await connection.execute("SELECT * FROM `ROUTE`");
      const [points, fields3] = await connection.execute("SELECT * FROM `POINT`");

      fileCount = files.length;
      routeCount = routes.length;
      pointCount = points.length;

  }catch(e){
      console.log("Query error: "+e);
      return res.status(400).send(e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    numFiles: fileCount,
    numRoutes: routeCount,
    numPoints: pointCount,
  });

});



app.get('/storeFilesEndpoint', async function(req,res){
  // let dbConf = {
  //   host     : 'dursley.socs.uoguelph.ca',
  //   user     : 'avargh01',
  //   password : '1098759',
  //   database : 'avargh01'
  // };

  console.log(dbConf);

  let fileCount = 0;
  let routeCount = 0;
  let pointCount = 0;

  const mysql = require('mysql2/promise');

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf);

      connection.execute("DELETE FROM POINT;");
      connection.execute("DELETE FROM ROUTE;");
      connection.execute("DELETE FROM FILE;");

      await connection.execute("CREATE TABLE IF NOT EXISTS FILE (gpx_id INT NOT NULL AUTO_INCREMENT,  file_name VARCHAR(60) NOT NULL,  ver DECIMAL(2,1), creator VARCHAR(256) NOT NULL, PRIMARY KEY(gpx_id) );");
  
      await connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT NOT NULL AUTO_INCREMENT,  route_name VARCHAR(256),  route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE);");

      await connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id INT NOT NULL AUTO_INCREMENT,  point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256),  route_len FLOAT(15,7) NOT NULL, route_id INT NOT NULL, PRIMARY KEY(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE);");

      let files = fs.readdirSync("uploads");

      

      // let JSONObjectArray = [];
      // for ()
      // NEED TO CHANGE THIS LOOP TO ALLOW ASYNC AWAIT FOR EXECUTE CALLS
      files.forEach(async (file) => {
        let filePath = 'uploads/' + file;

        let JSONparsed;
        let JSONstring = sharedLib.gpxJSON(filePath);
        // console.log(JSONstring +"\n");
        let jsonBool = sharedLib.validateGPXJSON(filePath,'parser/src/gpx.xsd');
        if (JSONstring !== "{}" && jsonBool === 1 ) {

          JSONparsed = JSON.parse(JSONstring);
          JSONparsed.fileName = file;

          let insertFile = "INSERT INTO FILE (file_name, ver, creator) VALUES ('" + JSONparsed.fileName + "',"+ JSONparsed.version + ",'" + JSONparsed.creator + "');";

          //Populate the table
          connection.execute(insertFile);

          //add routes database table
          let selectGPXID = "SELECT * FROM FILE WHERE file_name = '" + JSONparsed.fileName +"'";
          let selgpx = await connection.execute(selectGPXID);
          let cur_gpx_id = 0; //set from before

          // console.log(selgpx[0][0].gpx_id);
          cur_gpx_id = selgpx[0][0].gpx_id; //set from before


          // for ( let row of selgpx){
          //   cur_gpx_id = row.gpx_id;
          // }

          // console.log(cur_gpx_id);

          let routeListParsed;
          // console.log("validate: " + jsonBool);
          let JSONroutestring = sharedLib.routeListJSON(filePath);
          
          let routeNum = 1;

          if (JSONroutestring !== "[]") {

            routeListParsed = JSON.parse(JSONroutestring.replace("\n","\\n"));

            routeListParsed.forEach(async function(routeParsed){
              routeParsed.fileName = file;
              routeParsed.compName = "Route " + routeNum++;

              let insertRoute = "INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ('"+ routeParsed.name +"', " + routeParsed.len + "," + cur_gpx_id + ");";

              //Populate the table
              connection.execute(insertRoute);

              //go through waypoint list of routes
              let selectRouteID = "SELECT * FROM ROUTE WHERE route_name = '" + routeParsed.name +"'";
              let selRt = await connection.execute(selectRouteID);
              let cur_rt_id = 0; //set from before

              // console.log(selgpx[0][0].gpx_id);
              cur_rt_id = selRt[0][0].route_id;

              let wptListParsed;
              let JSONwptList = sharedLib.waypointListJSON(filePath, routeParsed.name);

              let wptIndex = 0;

              if (JSONwptList !== "[]"){
                wptListParsed = JSON.parse(JSONwptList.replace("\n","\\n"));

                wptListParsed.forEach(async function(pointParsed){

                  let insertPoint = "INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES ("+wptIndex+","+pointParsed.lat+","+pointParsed.lon+",'"+pointParsed.name+"'," + cur_rt_id +")";
                  wptIndex++;
                  connection.execute(insertPoint);

                })

              }

              
            }) 
                  
          }
        } 
        // else {
        //   fs.unlinkSync(filePath)
        // }
      });  
      
      const [selectFiles, fields1] = await connection.execute("SELECT * FROM `FILE`");
      const [routes, fields2] = await connection.execute("SELECT * FROM `ROUTE`");
      const [points, fields3] = await connection.execute("SELECT * FROM `POINT`");

      fileCount = selectFiles.length;
      routeCount = routes.length;
      pointCount = points.length;

  }catch(e){
      console.log("Query error: "+e);
      return res.status(500).send(e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    numFiles: fileCount,
    numRoutes: routeCount,
    numPoints: pointCount,
  });
});

app.get('/loginEndpoint', async function(req,res){
  dbConf = {
    host     : 'dursley.socs.uoguelph.ca',
    user     : req.query.username,
    password : req.query.password,
    database : req.query.database
  };

  let fileCount = 0;
  let routeCount = 0;
  let pointCount = 0;

  const mysql = require('mysql2/promise');
    
  let connection;

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf);

      connection.execute("CREATE TABLE IF NOT EXISTS FILE (gpx_id INT auto_increment,  file_name VARCHAR(60) NOT NULL,  ver DECIMAL(2,1), creator VARCHAR(256) NOT NULL, PRIMARY KEY(gpx_id) );");
  
      connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT auto_increment,  route_name VARCHAR(256),  route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE);");

      connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id INT auto_increment,  point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, PRIMARY KEY(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE);");

      const [files, fields1] = await connection.execute("SELECT * FROM `FILE`");
      const [routes, fields2] = await connection.execute("SELECT * FROM `ROUTE`");
      const [points, fields3] = await connection.execute("SELECT * FROM `POINT`");

      fileCount = files.length;
      routeCount = routes.length;
      pointCount = points.length;

  }catch(e){
      console.log("Query error: "+e);
      return res.status(400).send(e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    numFiles: fileCount,
    numRoutes: routeCount,
    numPoints: pointCount,
  });

});

app.get('/sortAllRoutesNameEndpoint', async function(req,res){
  // dbConf = {
  //   host     : 'dursley.socs.uoguelph.ca',
  //   user     : req.query.username,
  //   password : req.query.password,
  //   database : req.query.database
  // };

  let routeCount = 0;

  const mysql = require('mysql2/promise');
    
  let connection;

  let routeList = [];

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf);     
      

      // const [files, fields1] = await connection.execute("SELECT * FROM `FILE`");
      const [routes, fields2] = await connection.execute("SELECT * FROM `ROUTE` ORDER BY `route_name`");
      // const [points, fields3] = await connection.execute("SELECT * FROM `POINT`");

      routeCount = routes.length;

      

      for (let row of routes){
        let route = {};
        route.name = row.route_name;
        route.id = row.route_id;
        route.len = row.route_len;
        route.gpxID = row.gpx_id;
        // console.log(row.route_name);

        routeList.push(route);
      }
      console.log(routeList);

  }catch(e){
      console.log("Query error: "+e);
      return res.status(400).send(e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send(routeList);

});

app.get('/sortAllRoutesLengthEndpoint', async function(req,res){
  // dbConf = {
  //   host     : 'dursley.socs.uoguelph.ca',
  //   user     : req.query.username,
  //   password : req.query.password,
  //   database : req.query.database
  // };

  let routeCount = 0;

  const mysql = require('mysql2/promise');
    
  let connection;

  let routeList = [];

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf);     
      

      // const [files, fields1] = await connection.execute("SELECT * FROM `FILE`");
      const [routes, fields2] = await connection.execute("SELECT * FROM `ROUTE` ORDER BY `route_len`");
      // const [points, fields3] = await connection.execute("SELECT * FROM `POINT`");

      routeCount = routes.length;

      

      for (let row of routes){
        let route = {};
        route.name = row.route_name;
        route.id = row.route_id;
        route.len = row.route_len;
        route.gpxID = row.gpx_id;
        // console.log(row.route_name);

        routeList.push(route);
      }
      console.log(routeList);

  }catch(e){
      console.log("Query error: "+e);
      return res.status(400).send(e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send(routeList);

});