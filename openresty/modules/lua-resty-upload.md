# lua-resty-upload

> [openresty/lua-resty-upload: Streaming reader and parser for http file uploading](https://github.com/openresty/lua-resty-upload)



## 例子: 实现上传文件或者表单服务

```lua
--[[
    本文件用来存储使用 multipart/form-data 上传的文件
    multipart/form-data 可以同时上传多个表单和文件

-----------------------------196029770731369059433175012991
Content-Disposition: form-data; name="username"

user
-----------------------------196029770731369059433175012991
Content-Disposition: form-data; name="password"

pass123
-----------------------------196029770731369059433175012991
Content-Disposition: form-data; name="filename1"; filename="vcation.jpg"
Content-Type: image/jpeg
--]]

local upload = require('resty.upload')



-- 检测上传参数是否正确
local function _upload_check()
    -- 上传需要指定Content-Length 字段, 不支持chunked上传
    local content_length = ngx.req.get_headers()["Content-Length"]
    if not content_length then
        ngx.log("=== [Error] no Content-Length")
        return ngx.exit(400)
    end

    --[[ 判断上传body大小
        等于 0 :错误
        小于 0 :通常是文件过大(>4G) 类型溢出
        设置上传大小限制, 注意这个限制
    --]]
    local size = tonumber(content_length)
    if size <= 0 or size > 10485760 then
        return ngx.exit(413)
    end
end

--[[
文件上传状态切换:  header(Content-Disposition)->header(Content-Type)-> N x doby -> part_end
表单上传状态切换:  header(Content-Disposition)-> N x body -> part_end
body上传结束   :  eof
--]]


-----------------------------------------------------------------
local TYPE_FORM = 1
local TYPE_FILE = 2

-- 开始一个文件或表单的下载
-- 例子将文件和表单都存储到 current.value 中,实际应用中可以写入文件等
local function _store_start(current, type, name)
    current.type = type
    current.name = name
    current.value = ""

--[[ 大文件的时候可以写入文件, 发送到其他网络位置等
由于实际使用中,worker都属于 nginx 或者 nobody 等低权限用户组, 写文件时需要注意权限问题
    if type == TYPE_FILE then
        current.file = io.open("/some/path/" .. name, "w")
    end
--]]
end

-- 下载 文件 或 表单的value
local function _store_content(current, data)
    current.value = current.value .. data

--[[ 大文件的时候可以写入文件
    if type == TYPE_FILE then
        current.file:write(data)
    end
--]]
end

-- 一个文件或表单结束了, 这里就可以处理上传内容了
local function _store_end(current)
    if current.type == TYPE_FORM then
        ngx.log(ngx.ERR, "[FORM]", current.name, ":", current.value)
    else -- 文件
        --[[ 大文件的时候可以写入文件
        current.file:close()
        --]]
        ngx.log(ngx.ERR, "[FILE]", current.name, " length: ", string.len(current.value))
    end
end
-----------------------------------------------------------------



-- 开始下载上传文件
local function _download_file()
    local chunk_size = 8192

    local data, err = upload:new(chunk_size)
    if not data then
        ngx.log(ngx.ERR, err)
        return ngx.exit(500)
    end

    local current = table.new(3, 3)

    while true do
        local typ, res, err = data:read()
        if not typ then
            ngx.log(ngx.ERR, err)
            return ngx.exit(500)
        end

        if typ == "header" then
            if res[1] == "Content-Disposition" then -- 这里需要判断上传的是文件还是表单
                local filename = ngx.re.match( res[2], [=[filename="([^"]+)"]=]) -- 获取上传文件的名称
                if not filename then -- 不是文件就是表单了
                    local formname = ngx.re.match( res[2], [=[name="([^"]+)"]=]) -- 获取上传表单key
                    if not formname then
                        ngx.log(ngx.ERR, "=== [Error:unkown header]", res[2])
                        return ngx.exit(400)
                    else -- 准备读取表单
                        _store_start(current, TYPE_FORM, formname[1])
                    end
                else -- 准备读取文件
                    _store_start(current, TYPE_FILE, filename[1])
                end
            elseif res[1] == "Content-Type" then
                -- 可以做类型检查, 上传的文件是否是 image/jpeg 等
            else
                ngx.log(ngx.ERR, "never go here")
            end
        elseif typ == "body" then -- 文件或表单读取
            _store_content(current, res)
        elseif typ == "part_end" then -- 一个文件或表单结束
            _store_end(current)
        elseif typ == "eof" then
            return -- 所有上传数据处理完成, 退出循环
        else
            ngx.log(ngx.ERR, "never go here")
        end
    end
end

-- OpenResty 原生支持 application/x-www-form-urlencoded 格式的解析
local function _download_form()
    ngx.req.read_body()
    local args = ngx.req.get_post_args()
    for key, value in ipairs(args) do
        ngx.log(ngx.ERR, "[FORM]", key, ":", value)
    end
end

-- 普通的文件上传(使用了nginx原生的处理, 所以临时文件最好只读,不要随意修改)
local function _download_other()
    ngx.req.read_body()
    local data = ngx.req.get_body_data()
    if data then
        ngx.log(ngx.ERR, "[OTHER]", "in memory")
    else -- 内存中没有数据, 那么数据可能在临时文件里
        local file = ngx.req.get_body_file()
        if file then
            ngx.log(ngx.ERR, "[OTHER]", "in temp file:", file)
        else
            ngx.log(ngx.ERR, "[OTHER]", "error no data")
        end
    end
end
---------------------------------------------------------------
--- 程序入口
---------------------------------------------------------------
_upload_check()

-- 判断上传格式
local content_type = ngx.req.get_headers()["Content-Type"]
if not content_type then
    return ngx.exit(400)
end
if ngx.re.find(content_type,  [=[^multipart/form-data]=], "oij") then
    _download_file() -- 使用 form-data 格式上传文件或者表单
elseif content_type == "application/x-www-form-urlencoded" then
    _download_form()-- 使用 x-www-form-urlencoded 格式上传表单
else -- 其他类型的上传
    _download_other()
end


---- 成功应答
return ngx.print("upload ok")

```
