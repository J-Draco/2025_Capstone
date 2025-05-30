// content.js
const domain = window.location.hostname;
console.log("Content script 실행됨");

chrome.runtime.sendMessage({ type: "SEND_DOMAIN", domain: domain });
