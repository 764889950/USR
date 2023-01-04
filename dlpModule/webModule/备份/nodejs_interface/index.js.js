/* 使用nodejs编写api接口并部署到服务器上 http://wjhsh.net/tuspring-p-14340457.html  */
/* 引入express框架 */
const express = require('express');
const app = express();
/* 引入cors */
const cors = require('cors');
app.use(cors());
/* 引入body-parser */
const bodyParser = require('body-parser');
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

app.all('*', function (req, res, next) {
  if (!req.get('Origin')) return next();
  // use "*" here to accept any origin
  res.set('Access-Control-Allow-Origin', '*');
  res.set('Access-Control-Allow-Methods', 'GET');
  res.set('Access-Control-Allow-Headers', 'X-Requested-With, Content-Type');
  // res.set('Access-Control-Allow-Max-Age', 3600);
  if ('OPTIONS' == req.method) return res.send(200);
  next();
});

app.get('/', (req, res) => {
  res.send('<p style="color:red">服务已启动</p>');
})

app.get('/api/list', (req, res) => {
  res.json({
    code: 200,
    message: '成功',
    data: {
      list: []
    }
  });
})
/* 监听端口 */
app.listen(3000, () => {
  console.log('listen:3000');
})

