'use strict';

// Document Ready
$(document).ready(function () {
  M.AutoInit(document.body);
  nodeGrid();
});

// WebSocket URL Helper
const loc = window.location;
const wsLoc = new URL("./", `${loc.protocol}//${loc.host}${loc.pathname}`);
wsLoc.protocol = 'ws:';
if (loc.protocol === "https:") {
  wsLoc.protocol = 'wss:';
}
const wsUrl = wsLoc.href;


/**
 *  Dashboard Dynamic Grid
 */
const nodeGrid = () => {
  const nodeCount = $('#nodes .card').length
  const nodeGrid = $('#nodes #grid')

  if (nodeCount >= 6) {
    $(nodeGrid).addClass("grid-2-col");
  } else {
    $(nodeGrid).addClass("grid-1-col");
  }
}

/**
 *  Online/ Offline Status Indication
 */

let isFetchingPing = false;

const fetchPing = () => {
  if (isFetchingPing) {
    return;
  }

  isFetchingPing = true;

  fetch(`./ping`, { credentials: "same-origin" }).then(res => res.json())
    .then(response => {
      for (let filename in response) {
        let node = document.querySelector(`#nodes .card[data-node="${filename}"]`);

        if (node === null) {
          continue;
        }

        let status = response[filename];
        let className;

        if (status === null) {
          className = 'status-unknown';
        } else if (status === true) {
          className = 'status-online';
          node.setAttribute('data-last-connected', Date.now().toString());
        } else if (node.hasAttribute('data-last-connected')) {
          const attr = parseInt(node.getAttribute('data-last-connected'));
          if (Date.now() - attr <= 5000) {
            className = 'status-not-responding';
          } else {
            className = 'status-offline';
          }
        } else {
          className = 'status-offline';
        }

        if (node.classList.contains(className)) {
          continue;
        }

        node.classList.remove('status-unknown', 'status-online', 'status-offline', 'status-not-responding');
        node.classList.add(className);
      }

      isFetchingPing = false;
    });
};
setInterval(fetchPing, 2000);
fetchPing();

/**
 *  Log Color Parsing
 */

const initializeColorState = () => {
  return {
    bold: false,
    italic: false,
    underline: false,
    strikethrough: false,
    foregroundColor: false,
    backgroundColor: false,
    carriageReturn: false,
    secret: false,
  };
};

const colorReplace = (pre, state, text) => {
  const re = /(?:\033|\\033)(?:\[(.*?)[@-~]|\].*?(?:\007|\033\\))/g;
  let i = 0;

  if (state.carriageReturn) {
    if (text !== "\n") {
      // don't remove if \r\n
      pre.removeChild(pre.lastChild);
    }
    state.carriageReturn = false;
  }

  if (text.includes("\r")) {
    state.carriageReturn = true;
  }

  const lineSpan = document.createElement("span");
  lineSpan.classList.add("line");
  pre.appendChild(lineSpan);

  const addSpan = (content) => {
    if (content === "")
      return;

    const span = document.createElement("span");
    if (state.bold) span.classList.add("log-bold");
    if (state.italic) span.classList.add("log-italic");
    if (state.underline) span.classList.add("log-underline");
    if (state.strikethrough) span.classList.add("log-strikethrough");
    if (state.secret) span.classList.add("log-secret");
    if (state.foregroundColor !== null) span.classList.add(`log-fg-${state.foregroundColor}`);
    if (state.backgroundColor !== null) span.classList.add(`log-bg-${state.backgroundColor}`);
    span.appendChild(document.createTextNode(content));
    lineSpan.appendChild(span);

    if (state.secret) {
      const redacted = document.createElement("span");
      redacted.classList.add("log-secret-redacted");
      redacted.appendChild(document.createTextNode("[redacted]"));
      lineSpan.appendChild(redacted);
    }
  };


  while (true) {
    const match = re.exec(text);
    if (match === null)
      break;

    const j = match.index;
    addSpan(text.substring(i, j));
    i = j + match[0].length;

    if (match[1] === undefined) continue;

    for (const colorCode of match[1].split(";")) {
      switch (parseInt(colorCode)) {
        case 0:
          // reset
          state.bold = false;
          state.italic = false;
          state.underline = false;
          state.strikethrough = false;
          state.foregroundColor = null;
          state.backgroundColor = null;
          state.secret = false;
          break;
        case 1:
          state.bold = true;
          break;
        case 3:
          state.italic = true;
          break;
        case 4:
          state.underline = true;
          break;
        case 5:
          state.secret = true;
          break;
        case 6:
          state.secret = false;
          break;
        case 9:
          state.strikethrough = true;
          break;
        case 22:
          state.bold = false;
          break;
        case 23:
          state.italic = false;
          break;
        case 24:
          state.underline = false;
          break;
        case 29:
          state.strikethrough = false;
          break;
        case 30:
          state.foregroundColor = "black";
          break;
        case 31:
          state.foregroundColor = "red";
          break;
        case 32:
          state.foregroundColor = "green";
          break;
        case 33:
          state.foregroundColor = "yellow";
          break;
        case 34:
          state.foregroundColor = "blue";
          break;
        case 35:
          state.foregroundColor = "magenta";
          break;
        case 36:
          state.foregroundColor = "cyan";
          break;
        case 37:
          state.foregroundColor = "white";
          break;
        case 39:
          state.foregroundColor = null;
          break;
        case 41:
          state.backgroundColor = "red";
          break;
        case 42:
          state.backgroundColor = "green";
          break;
        case 43:
          state.backgroundColor = "yellow";
          break;
        case 44:
          state.backgroundColor = "blue";
          break;
        case 45:
          state.backgroundColor = "magenta";
          break;
        case 46:
          state.backgroundColor = "cyan";
          break;
        case 47:
          state.backgroundColor = "white";
          break;
        case 40:
        case 49:
          state.backgroundColor = null;
          break;
      }
    }
  }
  addSpan(text.substring(i));
  if (pre.scrollTop + 56 >= (pre.scrollHeight - pre.offsetHeight)) {
    // at bottom
    pre.scrollTop = pre.scrollHeight;
  }
};

/**
 *  Serial Port Selection
 */

const portSelect = document.querySelector('.nav-wrapper select');
let ports = [];

const fetchSerialPorts = (begin = false) => {
  fetch(`./serial-ports`, { credentials: "same-origin" }).then(res => res.json())
    .then(response => {
      if (ports.length === response.length) {
        let allEqual = true;
        for (let i = 0; i < response.length; i++) {
          if (ports[i].port !== response[i].port) {
            allEqual = false;
            break;
          }
        }
        if (allEqual)
          return;
      }
      const hasNewPort = response.length >= ports.length;

      ports = response;

      const inst = M.FormSelect.getInstance(portSelect);
      if (inst !== undefined) {
        inst.destroy();
      }

      portSelect.innerHTML = "";
      const prevSelected = getUploadPort();
      for (let i = 0; i < response.length; i++) {
        const val = response[i];
        if (val.port === prevSelected) {
          portSelect.innerHTML += `<option value="${val.port}" selected>${val.port} (${val.desc})</option>`;
        } else {
          portSelect.innerHTML += `<option value="${val.port}">${val.port} (${val.desc})</option>`;
        }
      }

      M.FormSelect.init(portSelect, {});
      if (!begin && hasNewPort)
        M.toast({ html: "Discovered new serial port." });
    });
};

const getUploadPort = () => {
  const inst = M.FormSelect.getInstance(portSelect);
  if (inst === undefined) {
    return "OTA";
  }

  inst._setSelectedStates();
  return inst.getSelectedValues()[0];
};
setInterval(fetchSerialPorts, 5000);
fetchSerialPorts(true);

/**
 * Log Elements
 */

// Log Modal Class
class LogModal {
  constructor({
    name,
    onPrepare = (modalElement, config) => { },
    onProcessExit = (modalElement, code) => { },
    onSocketClose = (modalElement) => { },
    dismissible = true
  }) {
    this.modalId = `js-${name}-modal`;
    this.dataAction = `${name}`;
    this.wsUrl = `${wsUrl}${name}`;
    this.dismissible = dismissible;
    this.activeFilename = null;

    this.modalElement = document.getElementById(this.modalId);
    this.nodeFilenameElement = document.querySelector(`#${this.modalId} #js-node-filename`);
    this.logElement = document.querySelector(`#${this.modalId} #js-log-area`);
    this.onPrepare = onPrepare;
    this.onProcessExit = onProcessExit;
    this.onSocketClose = onSocketClose;
  }

  setup() {
    const boundOnPress = this._onPress.bind(this);
    document.querySelectorAll(`[data-action="${this.dataAction}"]`).forEach((button) => {
      button.addEventListener('click', boundOnPress);
    });
  }

  _setupModalInstance() {
    this.modalInstance = M.Modal.init(this.modalElement, {
      onOpenStart: this._onOpenStart.bind(this),
      onCloseStart: this._onCloseStart.bind(this),
      dismissible: this.dismissible
    })
  }

  _onOpenStart() {
    document.addEventListener('keydown', this._boundKeydown);
  }

  _onCloseStart() {
    document.removeEventListener('keydown', this._boundKeydown);
    this.activeSocket.close();
  }

  open(event) {
    this._onPress(event);
  }

  _onPress(event) {
    this.activeFilename = event.target.getAttribute('data-filename');

    console.log(this.activeFilename);

    this._setupModalInstance();
    this.nodeFilenameElement.innerHTML = this.activeFilename;

    this.logElement.innerHTML = "";
    const colorLogState = initializeColorState();

    this.onPrepare(this.modalElement, this.activeFilename);

    let stopped = false;

    this.modalInstance.open();

    const socket = new WebSocket(this.wsUrl);
    this.activeSocket = socket;
    socket.addEventListener('message', (event) => {
      const data = JSON.parse(event.data);
      if (data.event === "line") {
        colorReplace(this.logElement, colorLogState, data.data);
      } else if (data.event === "exit") {
        this.onProcessExit(this.modalElement, data.code);
        stopped = true;
      }
    });

    socket.addEventListener('open', () => {
      const msg = JSON.stringify(this._encodeSpawnMessage(this.activeFilename));
      socket.send(msg);
    });

    socket.addEventListener('close', () => {
      if (!stopped) {
        this.onSocketClose(this.modalElement);
      }
    });
  }

  _onKeyDown(event) {
    // Close on escape key
    if (event.keyCode === 27) {
      this.modalInstance.close();
    }
  }

  _encodeSpawnMessage(filename) {
    return {
      type: 'spawn',
      configuration: filename,
      port: getUploadPort(),
    };
  }
}

// Logs Modal
const logsModal = new LogModal({
  name: "logs",

  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("[data-action='stop-logs']").innerHTML = "Stop";
  },

  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000
      });
    } else {
      M.toast({
        html: `Program failed with code ${code}`,
        displayLength: 10000
      });
    }
    modalElem.querySelector("data-action='stop-logs'").innerHTML = "Close";
  },

  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000
    });
  }
})

logsModal.setup();

// Upload Modal
const uploadModal = new LogModal({
  name: "upload",
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-upload-modal [data-action='download-binary']").classList.add('disabled');
    modalElement.querySelector("#js-upload-modal [data-action='upload']").setAttribute('data-filename', activeFilename);
    modalElement.querySelector("#js-upload-modal [data-action='upload']").classList.add('disabled');
    modalElement.querySelector("#js-upload-modal [data-action='edit']").setAttribute('data-filename', activeFilename);
    modalElement.querySelector("#js-upload-modal [data-action='stop-logs']").innerHTML = "Stop";
  },

  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000
      });

      modalElement.querySelector("#js-upload-modal [data-action='download-binary']").classList.remove('disabled');
    } else {
      M.toast({
        html: `Program failed with code ${code}`,
        displayLength: 10000
      });

      modalElement.querySelector("#js-upload-modal [data-action='upload']").classList.add('disabled');
      modalElement.querySelector("#js-upload-modal [data-action='upload']").classList.remove('disabled');
    }
    modalElement.querySelector("#js-upload-modal [data-action='stop-logs']").innerHTML = "Close";
  },

  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000
    });
  },

  dismissible: false
})

uploadModal.setup();

const downloadAfterUploadButton = document.querySelector("#js-upload-modal [data-action='download-binary']");
downloadAfterUploadButton.addEventListener('click', () => {
  const link = document.createElement("a");
  link.download = name;
  link.href = `./download.bin?configuration=${encodeURIComponent(uploadModal.activeFilename)}`;
  document.body.appendChild(link);
  link.click();
  link.remove();
});

// Validate Modal
const validateModal = new LogModal({
  name: 'validate',
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-validate-modal [data-action='stop-logs']").innerHTML = "Stop";
    modalElement.querySelector("#js-validate-modal [data-action='edit']").setAttribute('data-filename', activeFilename);
    modalElement.querySelector("#js-validate-modal [data-action='upload']").setAttribute('data-filename', activeFilename);
    modalElement.querySelector("#js-validate-modal [data-action='upload']").classList.add('disabled');
  },
  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: `<code class="inlinecode">${validateModal.activeFilename}</code> is valid 👍`,
        displayLength: 10000,
      });
      modalElement.querySelector("#js-validate-modal [data-action='upload']").classList.remove('disabled');
    } else {
      M.toast({
        html: `<code class="inlinecode">${validateModal.activeFilename}</code> is invalid 😕`,
        displayLength: 10000,
      });
    }
    modalElement.querySelector("#js-validate-modal [data-action='stop-logs']").innerHTML = "Close";
  },
  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000,
    });
  },
});

validateModal.setup();

// Compile Modal
const compileModal = new LogModal({
  name: 'compile',
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-compile-modal [data-action='stop-logs']").innerHTML = "Stop";
    modalElement.querySelector("#js-compile-modal [data-action='download-binary']").classList.add('disabled');
  },
  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000,
      });
      modalElement.querySelector("#js-compile-modal [data-action='download-binary']").classList.remove('disabled');
    } else {
      M.toast({
        html: `Program failed with code ${data.code}`,
        displayLength: 10000,
      });
    }
    modalElement.querySelector("#js-compile-modal [data-action='stop-logs']").innerHTML = "Close";
  },
  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000,
    });
  },
  dismissible: false,
});

compileModal.setup();

const downloadAfterCompileButton = document.querySelector("#js-compile-modal [data-action='download-binary']");
downloadAfterCompileButton.addEventListener('click', () => {
  const link = document.createElement("a");
  link.download = name;
  link.href = `./download.bin?configuration=${encodeURIComponent(compileModal.activeFilename)}`;
  document.body.appendChild(link);
  link.click();
  link.remove();
});

// Clean MQTT Modal
const cleanMqttModal = new LogModal({
  name: 'clean-mqtt',
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-clean-mqtt-modal [data-action='stop-logs']").innerHTML = "Stop";
  },
  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000,
      });
    } else {
      M.toast({
        html: `Program failed with code ${code}`,
        displayLength: 10000,
      });
    }
    modalElement.querySelector("#js-clean-mqtt-modal [data-action='stop-logs']").innerHTML = "Close";
  },
  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000,
    });
  },
});

cleanMqttModal.setup();

// Clean Build Files Modal
const cleanModal = new LogModal({
  name: 'clean',
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-clean-modal [data-action='stop-logs']").innerHTML = "Stop";
  },
  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000,
      });
    } else {
      M.toast({
        html: `Program failed with code ${code}`,
        displayLength: 10000,
      });
    }
    modalElement.querySelector("#js-clean-modal [data-action='stop-logs']").innerHTML = "Close";
  },
  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000,
    });
  },
});

cleanModal.setup();

// Update All Modal
const updateAllModal = new LogModal({
  name: 'update-all',
  onPrepare: (modalElement, activeFilename) => {
    modalElement.querySelector("#js-update-all-modal [data-action='stop-logs']").innerHTML = "Stop";
    modalElement.querySelector("#js-update-all-modal #js-node-filename").style.visibility = "hidden";
  },
  onProcessExit: (modalElement, code) => {
    if (code === 0) {
      M.toast({
        html: "Program exited successfully",
        displayLength: 10000,
      });
      downloadButton.classList.remove('disabled');
    } else {
      M.toast({
        html: `Program failed with code ${data.code}`,
        displayLength: 10000,
      });
    }
    modalElement.querySelector("#js-update-all-modal [data-action='stop-logs']").innerHTML = "Close";
  },
  onSocketClose: (modalElement) => {
    M.toast({
      html: 'Terminated process',
      displayLength: 10000,
    });
  },
  dismissible: false,
});

updateAllModal.setup();

/**
 *  Node Editing
 */

// ACE Validation
const editModalElem = document.getElementById("modal-editor");
const editorElem = editModalElem.querySelector("#editor");
const editor = ace.edit(editorElem);

let activeEditorConfig = null;
let activeEditorSecrets = false;
let aceWs = null;
let aceValidationScheduled = false;
let aceValidationRunning = false;

const startAceWebsocket = () => {
  aceWs = new WebSocket(`${wsUrl}ace`);
  aceWs.addEventListener('message', (event) => {
    const raw = JSON.parse(event.data);
    if (raw.event === "line") {
      const msg = JSON.parse(raw.data);
      if (msg.type === "result") {
        const arr = [];

        for (const v of msg.validation_errors) {
          let o = {
            text: v.message,
            type: 'error',
            row: 0,
            column: 0
          };
          if (v.range != null) {
            o.row = v.range.start_line;
            o.column = v.range.start_col;
          }
          arr.push(o);
        }
        for (const v of msg.yaml_errors) {
          arr.push({
            text: v.message,
            type: 'error',
            row: 0,
            column: 0
          });
        }

        editor.session.setAnnotations(arr);

        if (arr.length) {
          editorUploadButton.classList.add('disabled');
        } else {
          editorUploadButton.classList.remove('disabled');
        }

        aceValidationRunning = false;
      } else if (msg.type === "read_file") {
        sendAceStdin({
          type: 'file_response',
          content: editor.getValue()
        });
      }
    }
  });

  aceWs.addEventListener('open', () => {
    const msg = JSON.stringify({ type: 'spawn' });
    aceWs.send(msg);
  });

  aceWs.addEventListener('close', () => {
    aceWs = null;
    setTimeout(startAceWebsocket, 5000)
  });
};

const sendAceStdin = (data) => {
  let send = JSON.stringify({
    type: 'stdin',
    data: JSON.stringify(data) + '\n',
  });
  aceWs.send(send);
};

startAceWebsocket();

// Editor Settings / Commands
editor.setTheme("ace/theme/dreamweaver");
editor.session.setMode("ace/mode/yaml");
editor.session.setOption('useSoftTabs', true);
editor.session.setOption('tabSize', 2);
editor.session.setOption('useWorker', false);

const saveButton = editModalElem.querySelector(".save-button");
const editorUploadButton = editModalElem.querySelector(".editor-upload-button");

const saveEditor = () => {
  fetch(`./edit?configuration=${activeEditorConfig}`, {
    credentials: "same-origin",
    method: "POST",
    body: editor.getValue()
  }).then(res => res.text()).then(() => {
    M.toast({
      html: `Saved <code class="inlinecode">${activeEditorConfig}</code>`
    });
  });
};

const debounce = (func, wait) => {
  let timeout;
  return function () {
    let context = this, args = arguments;
    let later = function () {
      timeout = null;
      func.apply(context, args);
    };
    clearTimeout(timeout);
    timeout = setTimeout(later, wait);
  };
};

editor.commands.addCommand({
  name: 'saveCommand',
  bindKey: { win: 'Ctrl-S', mac: 'Command-S' },
  exec: saveEditor,
  readOnly: false
});

editor.session.on('change', debounce(() => {
  aceValidationScheduled = !activeEditorSecrets;
}, 250));

setInterval(() => {
  if (!aceValidationScheduled || aceValidationRunning)
    return;
  if (aceWs == null)
    return;

  sendAceStdin({
    type: 'validate',
    file: activeEditorConfig
  });
  aceValidationRunning = true;
  aceValidationScheduled = false;
}, 100);


// Edit Modal
saveButton.addEventListener('click', saveEditor);
editorUploadButton.addEventListener('click', saveEditor);

// HACK: Added temp code for new data-attributes
document.querySelectorAll(".action-edit, [data-action='edit']").forEach((btn) => {
  btn.addEventListener('click', (e) => {
    if (e.target.getAttribute('data-node')) {
      activeEditorConfig = e.target.getAttribute('data-node');
    }

    if (e.target.getAttribute('data-filename')) {
      activeEditorConfig = e.target.getAttribute('data-filename');
    }
    // activeEditorConfig = e.target.getAttribute('data-node');
    activeEditorSecrets = activeEditorConfig === 'secrets.yaml';
    const modalInstance = M.Modal.getInstance(editModalElem);
    const filenameField = editModalElem.querySelector('.filename');
    editorUploadButton.setAttribute('data-node', activeEditorConfig);
    if (activeEditorSecrets) {
      editorUploadButton.classList.add('disabled');
    }
    filenameField.innerHTML = activeEditorConfig;

    editor.setValue("Loading configuration yaml...");
    editor.setOption('readOnly', true);
    fetch(`./edit?configuration=${activeEditorConfig}`, { credentials: "same-origin" })
      .then(res => res.text()).then(response => {
        editor.setValue(response, -1);
        editor.setOption('readOnly', false);
      });

    modalInstance.open();
  });
});

// Delete Node
document.querySelectorAll(".action-delete").forEach((btn) => {
  btn.addEventListener('click', (e) => {
    let configuration = e.target.getAttribute('data-node');

    fetch(`./delete?configuration=${configuration}`, {
      credentials: "same-origin",
      method: "POST",
    }).then(res => res.text()).then(() => {
      const toastHtml = `<span>Deleted <code class="inlinecode">${configuration}</code>
                           <button class="btn-flat toast-action">Undo</button></button>`;
      const toast = M.toast({ html: toastHtml });
      const undoButton = toast.el.querySelector('.toast-action');

      document.querySelector(`.entry-row[data-node="${configuration}"]`).remove();

      undoButton.addEventListener('click', () => {
        fetch(`./undo-delete?configuration=${configuration}`, {
          credentials: "same-origin",
          method: "POST",
        }).then(res => res.text()).then(() => {
          window.location.reload(false);
        });
      });
    });
  });
});

//

/**
 *  Wizard
 */


const modalSetupElem = document.getElementById("modal-wizard");
const setupWizardStart = document.getElementById('setup-wizard-start');

const startWizard = () => {
  const modalInstance = M.Modal.getInstance(modalSetupElem);
  modalInstance.open();

  $('.stepper').activateStepper({
    linearStepsNavigation: false,
    autoFocusInput: true,
    autoFormCreation: true,
    showFeedbackLoader: true,
    parallel: false
  });
};

setupWizardStart.addEventListener('click', startWizard);

jQuery.validator.addMethod("nospaces", (value, element) => {
  return value.indexOf(' ') < 0;
}, "Name must not contain spaces.");

jQuery.validator.addMethod("lowercase", (value, element) => {
  return value === value.toLowerCase();
}, "Name must be lowercase.");

