/**  宏定义 **/
//面投影SLC解析地址1(本机)
CGIslcAnalyseIP1 = "192.168.0.104";
CGIslcAnalyseIP1Port = 8080;

/** 定义结束 **/

const express = require('express');
const app = express();

/*接收json数据*/
const bodyParser = require('body-parser');
const json = express.json({type: '*/json'});
app.use(json);
app.use(bodyParser.urlencoded({ extended: false }));
/*接收json数据配置结束*/

/*文件相关*/
const iconv = require('iconv-lite');/*中文编码*/
const fileUpload = require('./lib/index');
app.use('/form', express.static(__dirname + '/index.html'));
// default options
app.use(fileUpload());
/*文件相关配置结束*/

/*注册相关*/
const crypto = require("crypto");
const User = require("./models/User");
/*注册相关配置结束*/

/*调用子程序相关*/
const processId = require('child_process');
const axios = require("axios");
/*调用子程序相关配置结束*/

/*删除文件和查询文件是否存在*/
const del = require('delete');
const fileExists = require('file-exists');
/*删除文件和查询文件是否存在配置结束*/

/*以时间为序列的uid生成码相关包*/
const timerUid = require('timer-uid');
/*以时间为序列的uid生成码相关包*/

// 解析表单中间件
app.use(express.urlencoded({ extended: false }))

// req.query  req.body为请求接口时用户端传递的参数
app.get('/user', (req, res)=>{
    const body = req.query
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})


/*
休眠函数sleep
调用 await sleep(1500)
 */
function sleep(ms) {
    return new Promise(resolve=>setTimeout(resolve, ms))
}

/*
文件上传
 */
app.post('/file-express-fileupload', (req, res)=>{
  let sampleFile;
  let uploadPath;

  if (!req.files || Object.keys(req.files).length === 0) {
    res.status(400).send('No files were uploaded.');
    return;
  }

  console.log('req.files >>>', req.files); // eslint-disable-line

  sampleFile = req.files["sampleFile"];

  uploadPath = __dirname + '/uploads/' + sampleFile.name;
  iconv["skipDecodeWarning"] = true;//消除报警 新增20220918
  uploadPath = iconv.decode(uploadPath, 'utf-8');//新增20220802

  let promise = sleep(15000);
  
  let mv = sampleFile.mv(uploadPath, function(err) {
    if (err) {
      return res.status(500).send(err);
    }

    res.send('File uploaded to ' + uploadPath);
  });
  
  
//待添加解析与下载触发，完成下载后的删除触发
//   const child = processId.exec('ls', function(err, stdout,stderr){
//       console.log(err, stdout, stderr);
//   });

// const child = processId.exec('mosquitto_pub -t \'test/topic\' -m \'hello world\' -u "jhon" -P "123456" -h "192.168.0.104"', function(err, stdout,stderr){
//   console.log(err, stdout, stderr);
// });
})

/*
文件删除
 */
app.post('/file-delete', (req, res)=>{
/*
// async 异步操作
    del(['uploads/新建文本文档.txt'], function(err, deleted) {
        if (err) throw err;
        // deleted files
        console.log(deleted);
    });
*/
// sync 同步操作 当前用的多
    del.sync(['uploads/新建文本文档.txt']);

    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})

/*
查询文件是否存在
 */
app.post('/file-existsyesorno', (req, res)=> {

    console.log(fileExists.sync('uploads/bLayer13.png')) // OUTPUTS: true or false

    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})

/*
生成tuid
 */
app.post('/tuid', (req, res)=> {
    let tuid = timerUid.tuid();
    console.log(tuid);

    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})


/*
用户注册
 */
app.post('/db_reg', (req, res)=>{
    const body = req.body;
    //console.log("req:",req.headers);
    //console.log("req.body:",body);
    if(req.body["password-repeat"]!=req.body['password']) {
        console.log("两次输入的口令不一致");
        res.send({
            status: 200,
            data: body,
            message: '两次输入的口令不一致'
        })
        return;
    }

    //生成口令的散列值，我们使用md5加密
    let md5=crypto.createHash('md5');
    let password=md5.update(req.body.password).digest("base64");
    //声明需要添加的用户
    let newUser=new User({
        name:req.body.username,
        password:password
    });
    User.find(newUser.name,function(err,user){
        //如果用户已经存在
        if(user){
            console.log("该用户已经存在");
            res.send({
                status: 200,
                data: body,
                message: '该用户已经存在'
            })
			return;
        }
        //如果不存在则添加用户
        newUser.save(function(err){
            if(err){
                console.log("err");
				res.send({
					status: 500,
					message: 'err'
				})
                return;

            }

            console.log("注册成功");
            res.send({
                status: 200,
                data: body,
                message: '注册成功'
            })
        })
    })
})


app.post('/control', (req, res)=>{
    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})

/** nodejs http模块发送get与post请求 方法示例  http://www.qb5200.com/article/325340.html **/
/** GET请求 **/

/** POST请求 **/
//参考  https://blog.csdn.net/a1272146/article/details/123840899
function postMessage(bodyJson,IP,Port,Path,res_post)
{
    let querystring = require('querystring')
        , http = require('http');

    let data = JSON.stringify(bodyJson);

    let opt = {
        hostname:IP,// hostname:'192.168.0.104',
        port :Port, //port :8080,
        path:Path,  //path:'/cgi-bin/slcAnalyse.cgi',
        method: 'POST',
        headers: {
            'Content-Type':'application/json',
            'Content-Length': Buffer.byteLength(data)
        }
    };

    let req = http.request(opt, function (res) {
        res.on('data', function (data) {
            console.log(data.toString());
            res_post.send({
                status: 200,
                data: data.toString(),
                message: 'CGIslcAnalyse请求成功'
            })
        });
    });
    req.on('error', function(e) {
        console.log('problem with request: ' + e.message);
    });
    req.write(data);
    req.end();

}
/** ****************** **/

app.post('/CGIslcAnalyse', (req, res)=>{
    const body = req.body
    let data = postMessage(body,CGIslcAnalyseIP1,CGIslcAnalyseIP1Port,'/cgi-bin/slcAnalyse.cgi',res);
    res.send({
        status: 200,
        data: body,
        message: 'CGIslcAnalyse请求成功'
    })
})

app.post('/OTAupdateManage', (req, res)=>{
    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: 'OTAupdateManage请求成功'
    })
})

// 启动服务器
app.listen(8081, () => {
    console.log('express server running at http://127.0.0.1')
})

//如何在node.js应用程序中执行可执行文件   https://cloud.tencent.com/developer/ask/sof/1207400