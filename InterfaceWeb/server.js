var fs = require('fs');
var express = require('express');
var app = express();
const process = require('process');
const cp = require("child_process");
const multer  = require('multer')
const upload = multer({ dest: 'image/' })

app.use('/public',express.static('public'));
app.use('/image',express.static('image'));
app.listen('8080','0.0.0.0',()=>{
      console.log("server is listening on 8080 port");
});

app.get('/', function(req, res){
	res.setHeader('Content-Type', 'text/html');
	res.setHeader('Access-Control-Allow-Origin', '*');
	res.sendFile(__dirname + '/index.html');
});

app.post('/Execute', upload.single('imageFile'), function(req, res){
	if (fs.existsSync(__dirname + "/image/sortie.ppm")) {
		fs.unlinkSync(__dirname + "/image/sortie.ppm");
	}
	fs.rename("image/" + req.file.filename, "image/image.ppm", (err) => { 
		if (err) throw err;
		let params =  __dirname + "/image/image.ppm " + __dirname + "/image/sortie.ppm" + " " + req.query.split + " " + req.query.scale;
		
		if(process.platform.toLowerCase() === "win32"){
			commande = "start " + __dirname.substring(0,__dirname.lastIndexOf('\\')) + "\\build\\Release\\Mosaique.exe " + params ;
			/*if(!fs.existsSync( __dirname.substring(0,__dirname.lastIndexOf('\\')) + "\\build\\Release\\Mosaique.exe")){
				exec("Build.sh", (error, stdout, stderr) => {});
			}*/
		} else if (process.platform.toLowerCase() === "linux"){
			commande = __dirname.substring(0,__dirname.lastIndexOf('/')) + "/build/Mosaique " + params;
			/*if(!fs.existsSync( __dirname.substring(0,__dirname.lastIndexOf('\\')) + "/build/Mosaique")){
				exec("Build.sh", (error, stdout, stderr) => {});
			}*/
		}
		
		cp.exec(commande, (error, stdout, stderr) => {	
			res.end();
		});
	});
});
