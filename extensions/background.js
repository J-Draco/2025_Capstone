chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.type === "SEND_DOMAIN") {
    fetch("http://127.0.0.1:5000/receive_domain", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({ domain: message.domain })
    })
    .then(res => {
      console.log("raw res:", res);
      return res.text();  
    })
    .then(data => {
      console.log("서버 응답:", data);


      chrome.tabs.query({ active: true, currentWindow: true }, tabs => {
        chrome.scripting.executeScript({
          target: { tabId: tabs[0].id },
          func: (value) => {
            const pwInput = document.querySelector('input[type="password"]');
            if (pwInput) {
              pwInput.value = value;
              console.log("[확장프로그램] 비밀번호 자동 입력 완료");
            } else {
              console.warn("[확장프로그램] 비밀번호 입력창을 찾을 수 없음.");
            }
          },
          args: [data]  
        });
      });
    })
    .catch(err => {
      console.error("도메인 전송 실패:", err);
    });
  }
});
