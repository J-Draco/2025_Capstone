const btn = document.getElementById('sendBtn');
const status = document.getElementById('status');


chrome.tabs.query({ active: true, currentWindow: true }, tabs => {
  chrome.scripting.executeScript({
    target: { tabId: tabs[0].id },
    func: () => {
      const input = document.activeElement;
      return input && input.tagName === 'INPUT' && input.type === 'password';
    }
  }, (results) => {
    if (results && results[0].result === true) {
      btn.disabled = false;
      status.textContent = "입력창이 감지되었습니다.";
    } else {
      btn.disabled = true;
      status.textContent = "비밀번호 입력창을 선택해주세요.";
    }
  });
});


btn.addEventListener('click', () => {
  chrome.tabs.query({ active: true, currentWindow: true }, tabs => {
    const url = new URL(tabs[0].url); 
    const domain = url.hostname;      
    chrome.runtime.sendMessage({ type: "SEND_DOMAIN", domain: domain });
  });
});
