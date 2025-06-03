chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.type === "SEND_DOMAIN") {
    (async () => {
      try {
        const res = await fetch("http://127.0.0.1:5555/receive_domain", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ domain: message.domain, password: message.password })
        });

        const responseJson = await res.json();

        console.log("[DEBUG] final_hash from server:", responseJson.final_hash );
sendResponse({ final_hash: responseJson.final_hash }); 
      } catch (err) {
        console.error("Lockify 실패:", err);
        sendResponse({ final_hash: null });
      }
    })();

    return true; 
  }
});
