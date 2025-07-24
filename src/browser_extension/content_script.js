function injectPasswordButton() {
  const pwInput = document.querySelector('input[type="password"]');
  if (!pwInput) return;

  if (document.getElementById("lockify-button")) return;

  const createButton = () => {
    const button = document.createElement("button");
    button.id = "lockify-button";
    button.title = "Lockify 자동 입력";
    button.style.position = "absolute";
    button.style.right = "8px";
    button.style.top = "50%";
    button.style.transform = "translateY(-50%)";
    button.style.padding = "0";
    button.style.border = "none";
    button.style.background = "transparent";
    button.style.cursor = "pointer";
    button.style.zIndex = 9999;
    button.style.width = "60px";
    button.style.height = "60px";

    const img = document.createElement("img");
    img.src = chrome.runtime.getURL("lockify_icon.png");
    img.alt = "Lockify";
    img.style.width = "100%";
    img.style.height = "100%";
    button.appendChild(img);

    button.addEventListener("click", (event) => {
      event.preventDefault();

      const domain = window.location.hostname;
      const password = pwInput.value;

      chrome.runtime.sendMessage(
        { type: "SEND_DOMAIN", domain: domain, password: password },
        (response) => {
          if (chrome.runtime.lastError) {
            console.error(
              "[Lockify] 메시지 전송 오류:",
              chrome.runtime.lastError.message
            );
            return;
          }
          console.log("[Lockify] 입력 비밀번호:", password);
          console.log("[Lockify] 응답:", response);
          if (response && response.final_hash) {
            pwInput.value = response.final_hash;

            const form = pwInput.closest("form");
            if (form) {
              form.addEventListener("submit", (e) => e.preventDefault(), {
                once: true,
              });
            }

            console.log("[Lockify] 비밀번호 자동 입력 완료");
          }
        }
      );
    });

    pwInput.style.position = pwInput.style.position || "relative";
    pwInput.parentElement.style.position = "relative";
    pwInput.parentElement.appendChild(button);
  };

  const monitor = setInterval(() => {
    if (
      pwInput.value.trim() !== "" &&
      !document.getElementById("lockify-button")
    ) {
      createButton();
    } else if (pwInput.value.trim() === "") {
      const existing = document.getElementById("lockify-button");
      if (existing) existing.remove();
    }
  }, 200);
}

window.addEventListener("load", () => {
  setTimeout(injectPasswordButton, 500);
});
