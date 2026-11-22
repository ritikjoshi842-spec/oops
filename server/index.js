const express = require('express');
const cors = require('cors');
const { execFile, exec } = require('child_process');
const path = require('path');
const fs = require('fs');

const app = express();
const PORT = process.env.PORT || 5000;

app.use(cors());
app.use(express.json());

const ROOT_DIR = path.resolve(__dirname, '..');
const BIN_PATH = path.join(ROOT_DIR, 'bin', 'alf_ewg_engine.exe');

// Helper to run C++ engine executable
function runCppEngine(args) {
    return new Promise((resolve, reject) => {
        execFile(BIN_PATH, args, { cwd: ROOT_DIR }, (error, stdout, stderr) => {
            if (error) {
                // If it output valid json even with non-zero exit code or error
                try {
                    const parsed = JSON.parse(stdout);
                    return resolve(parsed);
                } catch (e) {
                    return reject({ error: error.message, stderr, stdout });
                }
            }
            try {
                const parsed = JSON.parse(stdout.trim());
                resolve(parsed);
            } catch (err) {
                // Return plain text if not JSON
                resolve({ rawOutput: stdout.trim(), stderr: stderr.trim() });
            }
        });
    });
}

// 1. Grid Status & Station Sweep
app.get('/api/status', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-status']);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 2. Trigger Sensor Sweep Cycle
app.post('/api/sweep', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-sweep']);
        res.json({ message: "Cyclical dynamic dispatch sensor sweep completed successfully.", ...data });
    } catch (err) {
        res.status(500).json(err);
    }
});

// 3. Evacuation Routes (Unit IV STL std::sort with Functor)
app.get('/api/routes', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-routes']);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 4. Binary Incidents (Unit IV std::fstream persistence)
app.get('/api/incidents', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-incidents']);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 5. Friend Class Auditor Deep Inspection
app.get('/api/audit/:stationId', async (req, res) => {
    try {
        const stationId = req.params.stationId || 'STA-KED-01';
        const data = await runCppEngine(['--json-audit', stationId]);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 6. Overloaded computeHazardIndex() Evaluation
app.post('/api/hazard/calculate', async (req, res) => {
    try {
        const { p1 = 85.0, p2 = 42.0, p3 = 30.0, p4 = 150.0 } = req.body;
        const data = await runCppEngine(['--json-hazard', String(p1), String(p2), String(p3), String(p4)]);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 7. Atomic Incident Logging
app.post('/api/incident/log', async (req, res) => {
    try {
        const { stationId = 'STA-KED-01', zone = 'Kedarnath Valley', hazard = 0.82, summary = 'Critical slope displacement alert' } = req.body;
        const data = await runCppEngine(['--json-log-incident', stationId, zone, String(hazard), summary]);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 8. Transactional seekp() Rollback Demonstration
app.post('/api/incident/test-rollback', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-test-rollback']);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 9. Fault Injection (Unit IV Custom Exceptions)
app.post('/api/fault/inject', async (req, res) => {
    try {
        const { faultType = 'sensor' } = req.body;
        const data = await runCppEngine(['--json-fault', faultType]);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 10. Operator + Cluster Merge
app.post('/api/cluster/merge', async (req, res) => {
    try {
        const data = await runCppEngine(['--json-cluster-merge']);
        res.json(data);
    } catch (err) {
        res.status(500).json(err);
    }
});

// 11. Full Academic Verification Suite Terminal Output
app.get('/api/academic-demo', (req, res) => {
    execFile(BIN_PATH, ['--demo'], { cwd: ROOT_DIR }, (error, stdout, stderr) => {
        res.json({ output: stdout, stderr });
    });
});

// Serve frontend build if exists
const FRONTEND_DIST = path.join(ROOT_DIR, 'ui', 'dist');
if (fs.existsSync(FRONTEND_DIST)) {
    app.use(express.static(FRONTEND_DIST));
    app.get('*', (req, res) => {
        res.sendFile(path.join(FRONTEND_DIST, 'index.html'));
    });
}

app.listen(PORT, () => {
    console.log(`[ALF-EWG Backend] Express C++ Bridge listening on http://localhost:${PORT}`);
    console.log(`[ALF-EWG Backend] C++ Core Binary bound at: ${BIN_PATH}`);
});
