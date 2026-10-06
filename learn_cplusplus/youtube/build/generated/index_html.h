// Generated from src/index.html; do not edit.
static const char *kIndexHtml = R"WEBVIEW_HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8" />
  <title>webview 测试项目</title>
  <style>
    body { font-family: system-ui, sans-serif; margin: 1.5rem;
           background: #1e1f22; color: #e6e6e6; }
    h1 { font-size: 1.1rem; margin-top: 0; }
    .row { margin: .9rem 0; display: flex; align-items: center; gap: .5rem; }
    input { padding: .35rem .5rem; border-radius: 6px; border: 1px solid #555;
            background: #2b2d31; color: inherit; }
    button { padding: .35rem .8rem; border-radius: 6px; border: 1px solid #555;
             background: #383a40; color: inherit; cursor: pointer; }
    button:hover { background: #45474d; }
    b { color: #9fd3ff; }
    #log { margin-top: 1.2rem; font-family: monospace; font-size: .8rem;
           white-space: pre-wrap; color: #8fbf8f; }
  </style>
</head>
<body>
  <h1>webview 测试项目</h1>
  <p>webview 版本：<b id="version">…</b></p>

  <div class="row">
    <input id="name" type="text" value="世界" />
    <button id="greetBtn">向 C++ 打招呼</button>
    <span><b id="greetResult">?</b></span>
  </div>

  <div id="log"></div>

<script>
  const $ = (id) => document.getElementById(id);
  const log = (text) => { $("log").textContent += text + "\n"; };

  // 由 C++ 绑定的函数返回的是 Promise，用 await 拿结果
  window.app_version().then((v) => { $("version").textContent = v; });

  $("greetBtn").addEventListener("click", async () => {
    const result = await window.greet($("name").value);
    $("greetResult").textContent = result;
    log("C++ 返回 greet -> " + result);
  });
</script>
</body>
</html>
)WEBVIEW_HTML";
