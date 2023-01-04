const express = require('express');
const app = express();

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

app.post('/my', (req, res)=>{
    const body = req.body
    res.send({
        status: 200,
        data: body,
        message: '请求成功'
    })
})

// 启动服务器
app.listen(80, () => {
    console.log('express server running at http://127.0.0.1')
})