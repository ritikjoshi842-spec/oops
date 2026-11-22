import React, { useState, useEffect } from 'react';
import { 
  ShieldAlert, Activity, Cpu, Database, Radio, RefreshCw, AlertTriangle, 
  Layers, HardDrive, Terminal, Zap, Compass, CheckCircle2, XCircle, 
  Flame, Sun, Volume2, VolumeX, Eye, ArrowRight, Play, Server
} from 'lucide-react';
import './App.css';

const API_BASE = 'http://localhost:5000/api';

export default function App() {
  const [activeTab, setActiveTab] = useState('grid');
  const [gridData, setGridData] = useState(null);
  const [routes, setRoutes] = useState([]);
  const [incidents, setIncidents] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selectedStation, setSelectedStation] = useState(null);

  // Unit 1 Overload Simulator State
  const [hazardParams, setHazardParams] = useState({ p1: 85, p2: 42, p3: 30, p4: 150 });
  const [hazardResult, setHazardResult] = useState(null);

  // Unit 1 Auditor State
  const [auditData, setAuditData] = useState(null);
  const [auditStationId, setAuditStationId] = useState('STA-KED-01');

  // Unit 2 Cluster Merge State
  const [clusterMergeResult, setClusterMergeResult] = useState(null);

  // Unit 4 Rollback & Fault States
  const [rollbackResult, setRollbackResult] = useState(null);
  const [faultResult, setFaultResult] = useState(null);
  const [demoOutput, setDemoOutput] = useState('');

  // Fetch initial data
  useEffect(() => {
    fetchGridStatus();
    fetchRoutes();
    fetchIncidents();
  }, []);

  const fetchGridStatus = async () => {
    setLoading(true);
    try {
      const res = await fetch(`${API_BASE}/status`);
      const data = await res.json();
      setGridData(data);
      if (data.stations && data.stations.length > 0 && !selectedStation) {
        setSelectedStation(data.stations[0]);
      }
    } catch (err) {
      console.error("Failed to fetch grid status:", err);
    } finally {
      setLoading(false);
    }
  };

  const triggerSweep = async () => {
    setLoading(true);
    try {
      const res = await fetch(`${API_BASE}/sweep`, { method: 'POST' });
      const data = await res.json();
      setGridData(data);
      if (data.stations && selectedStation) {
        const updated = data.stations.find(s => s.id === selectedStation.id);
        if (updated) setSelectedStation(updated);
      }
    } catch (err) {
      console.error("Failed to trigger sweep:", err);
    } finally {
      setLoading(false);
    }
  };

  const fetchRoutes = async () => {
    try {
      const res = await fetch(`${API_BASE}/routes`);
      const data = await res.json();
      setRoutes(data.routes || []);
    } catch (err) {
      console.error("Failed to fetch routes:", err);
    }
  };

  const fetchIncidents = async () => {
    try {
      const res = await fetch(`${API_BASE}/incidents`);
      const data = await res.json();
      setIncidents(data.records || []);
    } catch (err) {
      console.error("Failed to fetch incidents:", err);
    }
  };

  const runAudit = async (stationId) => {
    try {
      const res = await fetch(`${API_BASE}/audit/${stationId}`);
      const data = await res.json();
      setAuditData(data);
    } catch (err) {
      console.error("Audit failed:", err);
    }
  };

  const runHazardCalculation = async () => {
    try {
      const res = await fetch(`${API_BASE}/hazard/calculate`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(hazardParams)
      });
      const data = await res.json();
      setHazardResult(data);
    } catch (err) {
      console.error("Hazard calculation failed:", err);
    }
  };

  const runClusterMerge = async () => {
    try {
      const res = await fetch(`${API_BASE}/cluster/merge`, { method: 'POST' });
      const data = await res.json();
      setClusterMergeResult(data);
    } catch (err) {
      console.error("Cluster merge failed:", err);
    }
  };

  const runRollbackTest = async () => {
    try {
      const res = await fetch(`${API_BASE}/incident/test-rollback`, { method: 'POST' });
      const data = await res.json();
      setRollbackResult(data);
      fetchIncidents();
    } catch (err) {
      console.error("Rollback test failed:", err);
    }
  };

  const runFaultInjection = async (faultType) => {
    try {
      const res = await fetch(`${API_BASE}/fault/inject`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ faultType })
      });
      const data = await res.json();
      setFaultResult(data);
    } catch (err) {
      console.error("Fault injection failed:", err);
    }
  };

  const fetchDemoOutput = async () => {
    setLoading(true);
    try {
      const res = await fetch(`${API_BASE}/academic-demo`);
      const data = await res.json();
      setDemoOutput(data.output || data.stderr);
    } catch (err) {
      console.error("Demo failed:", err);
    } finally {
      setLoading(false);
    }
  };

  const getAlertBadgeClass = (level) => {
    switch (level) {
      case 'RED_EVACUATE': return 'badge-red';
      case 'ORANGE_WARNING': return 'badge-orange';
      case 'YELLOW_ADVISORY': return 'badge-yellow';
      default: return 'badge-green';
    }
  };

  return (
    <div style={{ minHeight: '100vh', display: 'flex', flexDirection: 'column' }}>
      {/* Top Navigation Bar */}
      <header style={{
        background: 'rgba(17, 24, 39, 0.95)',
        borderBottom: '1px solid var(--border-subtle)',
        padding: '14px 28px',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        position: 'sticky',
        top: 0,
        zIndex: 100,
        backdropFilter: 'blur(10px)'
      }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px' }}>
          <div style={{
            width: '42px',
            height: '42px',
            borderRadius: '10px',
            background: 'linear-gradient(135deg, #0284c7, #2563eb)',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            boxShadow: '0 0 15px rgba(56, 189, 248, 0.4)'
          }}>
            <ShieldAlert size={26} color="#fff" />
          </div>
          <div>
            <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
              <h1 style={{ fontSize: '1.25rem', fontWeight: 800, letterSpacing: '-0.02em' }}>
                ALF-EWG COMMAND GRID
              </h1>
              <span className="badge badge-green" style={{ fontSize: '0.65rem' }}>
                <span className="radar-blip" style={{ color: '#10b981' }}></span>
                C++ CORE ACTIVE
              </span>
            </div>
            <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>
              Autonomous Landslide & Flash-Flood Early Warning Grid | Himalayan Telemetry Sector
            </p>
          </div>
        </div>

        {/* Global Network Counters (Unit II Static Members) */}
        <div style={{ display: 'flex', alignItems: 'center', gap: '20px' }}>
          <div style={{
            background: 'var(--bg-card)',
            padding: '6px 14px',
            borderRadius: '8px',
            border: '1px solid var(--border-subtle)',
            textAlign: 'center'
          }}>
            <div style={{ fontSize: '0.65rem', color: 'var(--text-dim)', textTransform: 'uppercase', fontWeight: 700 }}>
              Static Active Stations
            </div>
            <div className="mono" style={{ fontSize: '1.1rem', fontWeight: 700, color: 'var(--accent-cyan)' }}>
              {gridData?.activeStationCount ?? 5} Nodes
            </div>
          </div>

          <div style={{
            background: 'var(--bg-card)',
            padding: '6px 14px',
            borderRadius: '8px',
            border: '1px solid var(--border-subtle)',
            textAlign: 'center'
          }}>
            <div style={{ fontSize: '0.65rem', color: 'var(--text-dim)', textTransform: 'uppercase', fontWeight: 700 }}>
              Global Alert Level
            </div>
            <div className="mono" style={{
              fontSize: '1.1rem',
              fontWeight: 700,
              color: gridData?.globalEmergencyLevel >= 2 ? 'var(--accent-rose)' : 'var(--accent-amber)'
            }}>
              LEVEL {gridData?.globalEmergencyLevel ?? 3}
            </div>
          </div>

          <button 
            className="btn btn-primary"
            onClick={triggerSweep}
            disabled={loading}
          >
            <RefreshCw size={16} className={loading ? "spin" : ""} />
            Dynamic Dispatch Sweep
          </button>
        </div>
      </header>

      {/* Navigation Tabs */}
      <nav style={{
        background: 'var(--bg-secondary)',
        borderBottom: '1px solid var(--border-subtle)',
        padding: '0 28px',
        display: 'flex',
        gap: '4px'
      }}>
        {[
          { id: 'grid', label: 'Grid Telemetry', icon: Activity },
          { id: 'unit1', label: 'Unit I: Core & Call Semantics', icon: Cpu },
          { id: 'unit2', label: 'Unit II: Memory & Operators', icon: Layers },
          { id: 'unit3', label: 'Unit III: Diamond & Polymorphism', icon: Radio },
          { id: 'unit4', label: 'Unit IV: Persistence & STL', icon: Database },
          { id: 'terminal', label: 'C++ Academic Verification', icon: Terminal }
        ].map(tab => {
          const Icon = tab.icon;
          const isActive = activeTab === tab.id;
          return (
            <button
              key={tab.id}
              onClick={() => {
                setActiveTab(tab.id);
                if (tab.id === 'terminal' && !demoOutput) fetchDemoOutput();
              }}
              style={{
                display: 'flex',
                alignItems: 'center',
                gap: '8px',
                padding: '14px 18px',
                background: 'transparent',
                border: 'none',
                borderBottom: isActive ? '3px solid var(--accent-cyan)' : '3px solid transparent',
                color: isActive ? 'var(--accent-cyan)' : 'var(--text-muted)',
                fontWeight: isActive ? 700 : 500,
                fontSize: '0.875rem',
                cursor: 'pointer',
                transition: 'all 0.15s ease'
              }}
            >
              <Icon size={18} />
              {tab.label}
            </button>
          );
        })}
      </nav>

      {/* Main Content Area */}
      <main style={{ flex: 1, padding: '28px', maxWidth: '1600px', margin: '0 auto', width: '100%' }}>
        
        {/* ================================================================= */}
        {/* TAB 1: GRID TELEMETRY OVERVIEW                                    */}
        {/* ================================================================= */}
        {activeTab === 'grid' && (
          <div>
            <div style={{ marginBottom: '24px', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
              <div>
                <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>Himalayan Early Warning Stations</h2>
                <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                  Real-time telemetry streams from high-altitude IoT nodes monitoring pore pressure, vibrations, sonar river levels & acoustic emissions.
                </p>
              </div>
              <div style={{ display: 'flex', gap: '10px' }}>
                <span className="badge badge-green">Green (Nominal)</span>
                <span className="badge badge-yellow">Yellow (Advisory)</span>
                <span className="badge badge-orange">Orange (Warning)</span>
                <span className="badge badge-red">Red (Evacuate)</span>
              </div>
            </div>

            {/* Station Cards Grid */}
            <div className="dashboard-grid" style={{ marginBottom: '28px' }}>
              {gridData?.stations?.map(st => {
                const isSelected = selectedStation?.id === st.id;
                const isRed = st.alertLevel === 'RED_EVACUATE';
                return (
                  <div 
                    key={st.id}
                    onClick={() => setSelectedStation(st)}
                    className={`glass-panel ${isRed ? 'glow-rose' : ''}`}
                    style={{
                      padding: '20px',
                      cursor: 'pointer',
                      border: isSelected ? '2px solid var(--accent-cyan)' : '1px solid var(--border-subtle)',
                      transition: 'transform 0.15s ease, border-color 0.15s ease'
                    }}
                  >
                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: '12px' }}>
                      <div>
                        <div className="mono" style={{ fontSize: '0.75rem', color: 'var(--accent-cyan)', fontWeight: 600 }}>
                          {st.id}
                        </div>
                        <h3 style={{ fontSize: '1.1rem', fontWeight: 700 }}>{st.name}</h3>
                      </div>
                      <span className={`badge ${getAlertBadgeClass(st.alertLevel)}`}>
                        {st.alertLevel.replace('_', ' ')}
                      </span>
                    </div>

                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '16px', fontSize: '0.8rem', color: 'var(--text-dim)' }}>
                      <span>Alt: <strong style={{ color: 'var(--text-main)' }}>{st.elevation}m</strong></span>
                      <span>UTM: <strong style={{ color: 'var(--text-main)' }}>Zone {st.utmZone}N</strong></span>
                      <span>Nodes: <strong style={{ color: 'var(--text-main)' }}>{st.sensorNodes.length}</strong></span>
                    </div>

                    {/* Hazard Meter Bar */}
                    <div style={{ marginBottom: '16px' }}>
                      <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: '0.75rem', marginBottom: '4px' }}>
                        <span style={{ color: 'var(--text-muted)' }}>Hazard Assessment Score</span>
                        <span className="mono" style={{ fontWeight: 700, color: st.hazardScore > 0.7 ? 'var(--accent-rose)' : 'var(--accent-cyan)' }}>
                          {(st.hazardScore * 100).toFixed(0)}% ({st.hazardScore.toFixed(2)})
                        </span>
                      </div>
                      <div style={{ height: '8px', background: 'rgba(255,255,255,0.06)', borderRadius: '4px', overflow: 'hidden' }}>
                        <div style={{
                          height: '100%',
                          width: `${Math.min(st.hazardScore * 100, 100)}%`,
                          background: st.hazardScore >= 0.75 ? '#f43f5e' : st.hazardScore >= 0.5 ? '#f97316' : st.hazardScore >= 0.25 ? '#f59e0b' : '#10b981',
                          borderRadius: '4px',
                          transition: 'width 0.5s ease'
                        }}></div>
                      </div>
                    </div>

                    {/* Siren and Buffer Summary */}
                    <div style={{
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'space-between',
                      padding: '8px 12px',
                      background: 'rgba(0,0,0,0.25)',
                      borderRadius: '8px',
                      fontSize: '0.75rem'
                    }}>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                        {st.siren.sounding ? (
                          <span style={{ color: '#f43f5e', display: 'flex', alignItems: 'center', gap: '4px', fontWeight: 700 }}>
                            <Volume2 size={16} /> SIREN ACTIVE ({st.siren.decibels}dB)
                          </span>
                        ) : (
                          <span style={{ color: 'var(--text-dim)', display: 'flex', alignItems: 'center', gap: '4px' }}>
                            <VolumeX size={16} /> Siren Idle
                          </span>
                        )}
                      </div>
                      <span className="mono" style={{ color: 'var(--accent-cyan)' }}>
                        Buffer: {st.bufferSize}/{st.bufferCapacity}
                      </span>
                    </div>
                  </div>
                );
              })}
            </div>

            {/* Selected Station Deep Details */}
            {selectedStation && (
              <div className="glass-panel" style={{ padding: '24px', borderTop: '4px solid var(--accent-cyan)' }}>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '20px' }}>
                  <div>
                    <span className="badge badge-green" style={{ marginBottom: '8px' }}>
                      Selected Telemetry Unit
                    </span>
                    <h3 style={{ fontSize: '1.4rem', fontWeight: 800 }}>
                      {selectedStation.name} ({selectedStation.id})
                    </h3>
                  </div>
                  <div style={{ display: 'flex', gap: '10px' }}>
                    <button 
                      className="btn btn-secondary"
                      onClick={() => runAudit(selectedStation.id)}
                    >
                      <Eye size={16} />
                      Inspect Private Registers (Friend Auditor)
                    </button>
                  </div>
                </div>

                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', gap: '20px' }}>
                  {/* Coordinates & Projections */}
                  <div style={{ background: 'rgba(0,0,0,0.3)', padding: '16px', borderRadius: '8px' }}>
                    <h4 style={{ fontSize: '0.85rem', color: 'var(--accent-cyan)', marginBottom: '12px', display: 'flex', alignItems: 'center', gap: '6px' }}>
                      <Compass size={16} /> Geocoordinate Transformations (Call & Return by Ref)
                    </h4>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', fontSize: '0.85rem' }}>
                      <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                        <span style={{ color: 'var(--text-muted)' }}>WGS84 Coordinates:</span>
                        <span className="mono">{selectedStation.latitude}°N, {selectedStation.longitude}°E</span>
                      </div>
                      <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                        <span style={{ color: 'var(--text-muted)' }}>Elevation Above MSL:</span>
                        <span className="mono">{selectedStation.elevation} meters</span>
                      </div>
                      <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                        <span style={{ color: 'var(--text-muted)' }}>UTM Zone 44N Easting:</span>
                        <span className="mono">{selectedStation.utmEasting} m</span>
                      </div>
                      <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                        <span style={{ color: 'var(--text-muted)' }}>UTM Zone 44N Northing:</span>
                        <span className="mono">{selectedStation.utmNorthing} m</span>
                      </div>
                    </div>
                  </div>

                  {/* Connected Sensors (Unit III Dynamic Dispatch) */}
                  <div style={{ background: 'rgba(0,0,0,0.3)', padding: '16px', borderRadius: '8px' }}>
                    <h4 style={{ fontSize: '0.85rem', color: 'var(--accent-cyan)', marginBottom: '12px', display: 'flex', alignItems: 'center', gap: '6px' }}>
                      <Radio size={16} /> Dynamic Dispatch Nodes (vtable / _vptr bound)
                    </h4>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
                      {selectedStation.sensorNodes.map(node => (
                        <div key={node.id} style={{
                          padding: '10px',
                          background: 'rgba(255,255,255,0.03)',
                          borderRadius: '6px',
                          border: '1px solid var(--border-subtle)',
                          display: 'flex',
                          justifyContent: 'space-between',
                          alignItems: 'center'
                        }}>
                          <div>
                            <div className="mono" style={{ fontSize: '0.75rem', color: 'var(--accent-blue)', fontWeight: 700 }}>
                              {node.id}
                            </div>
                            <div style={{ fontSize: '0.8rem', color: 'var(--text-main)', fontWeight: 600 }}>
                              {node.type.replace(/_/g, ' ')}
                            </div>
                            <div style={{ fontSize: '0.7rem', color: 'var(--text-dim)' }}>
                              Composed PowerSubsystem: {node.power.voltage}V | {node.power.state}
                            </div>
                          </div>
                          <div className="mono" style={{ fontSize: '1.1rem', fontWeight: 700, color: 'var(--accent-emerald)' }}>
                            {node.value}
                          </div>
                        </div>
                      ))}
                    </div>
                  </div>

                  {/* Auditor Probe Output */}
                  {auditData && auditData.stationId === selectedStation.id && (
                    <div style={{
                      gridColumn: '1 / -1',
                      background: 'rgba(2, 6, 23, 0.8)',
                      border: '1px solid var(--accent-cyan)',
                      padding: '16px',
                      borderRadius: '8px'
                    }}>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px', color: 'var(--accent-cyan)' }}>
                        <Terminal size={18} />
                        <strong style={{ fontSize: '0.9rem' }}>Friend Class TelemetryAuditor Memory Output</strong>
                      </div>
                      <pre className="mono" style={{
                        fontSize: '0.8rem',
                        color: '#38bdf8',
                        whiteSpace: 'pre-wrap',
                        lineHeight: 1.4
                      }}>
                        {auditData.auditReport}
                      </pre>
                    </div>
                  )}
                </div>
              </div>
            )}
          </div>
        )}

        {/* ================================================================= */}
        {/* TAB 2: UNIT I - CORE FOUNDATIONS & CALL SEMANTICS                  */}
        {/* ================================================================= */}
        {activeTab === 'unit1' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
            <div>
              <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>Unit I: Core Foundations & Call Semantics</h2>
              <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                Encapsulation into namespaces, inline risk algorithms, overloaded computeHazardIndex(), call & return by reference, and friend auditor inspection.
              </p>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(450px, 1fr))', gap: '24px' }}>
              {/* Overload Function Simulator */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Zap size={20} /> Overloaded computeHazardIndex() Sandbox
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  The C++ engine defines multiple operational overloads to evaluate diverse telemetry combinations in real-time.
                </p>

                <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '12px', marginBottom: '20px' }}>
                  <div>
                    <label style={{ fontSize: '0.75rem', color: 'var(--text-dim)', display: 'block', marginBottom: '4px' }}>
                      Rainfall / Saturation (mm/h or %)
                    </label>
                    <input 
                      type="number"
                      value={hazardParams.p1}
                      onChange={e => setHazardParams({ ...hazardParams, p1: parseFloat(e.target.value) || 0 })}
                      style={{
                        width: '100%',
                        padding: '8px 12px',
                        background: 'rgba(0,0,0,0.4)',
                        border: '1px solid var(--border-subtle)',
                        borderRadius: '6px',
                        color: '#fff',
                        fontFamily: 'var(--font-mono)'
                      }}
                    />
                  </div>

                  <div>
                    <label style={{ fontSize: '0.75rem', color: 'var(--text-dim)', display: 'block', marginBottom: '4px' }}>
                      Slope Angle / Vibration (deg or Hz)
                    </label>
                    <input 
                      type="number"
                      value={hazardParams.p2}
                      onChange={e => setHazardParams({ ...hazardParams, p2: parseFloat(e.target.value) || 0 })}
                      style={{
                        width: '100%',
                        padding: '8px 12px',
                        background: 'rgba(0,0,0,0.4)',
                        border: '1px solid var(--border-subtle)',
                        borderRadius: '6px',
                        color: '#fff',
                        fontFamily: 'var(--font-mono)'
                      }}
                    />
                  </div>

                  <div>
                    <label style={{ fontSize: '0.75rem', color: 'var(--text-dim)', display: 'block', marginBottom: '4px' }}>
                      Pore Pressure / River Level (kPa or m)
                    </label>
                    <input 
                      type="number"
                      value={hazardParams.p3}
                      onChange={e => setHazardParams({ ...hazardParams, p3: parseFloat(e.target.value) || 0 })}
                      style={{
                        width: '100%',
                        padding: '8px 12px',
                        background: 'rgba(0,0,0,0.4)',
                        border: '1px solid var(--border-subtle)',
                        borderRadius: '6px',
                        color: '#fff',
                        fontFamily: 'var(--font-mono)'
                      }}
                    />
                  </div>

                  <div>
                    <label style={{ fontSize: '0.75rem', color: 'var(--text-dim)', display: 'block', marginBottom: '4px' }}>
                      Acoustic Emission (counts/min)
                    </label>
                    <input 
                      type="number"
                      value={hazardParams.p4}
                      onChange={e => setHazardParams({ ...hazardParams, p4: parseFloat(e.target.value) || 0 })}
                      style={{
                        width: '100%',
                        padding: '8px 12px',
                        background: 'rgba(0,0,0,0.4)',
                        border: '1px solid var(--border-subtle)',
                        borderRadius: '6px',
                        color: '#fff',
                        fontFamily: 'var(--font-mono)'
                      }}
                    />
                  </div>
                </div>

                <button className="btn btn-primary" onClick={runHazardCalculation} style={{ width: '100%', marginBottom: '16px' }}>
                  <Play size={16} /> Execute C++ Overload Functions
                </button>

                {hazardResult && (
                  <div style={{ background: 'rgba(0,0,0,0.3)', padding: '16px', borderRadius: '8px', border: '1px solid var(--border-subtle)' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px', alignItems: 'center' }}>
                      <span style={{ fontSize: '0.85rem' }}>Overload A (Geotechnical Risk):</span>
                      <span className="mono" style={{ fontWeight: 700, color: 'var(--accent-amber)' }}>
                        {hazardResult.hazardGeotechnical} ({hazardResult.alertGeotechnical})
                      </span>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                      <span style={{ fontSize: '0.85rem' }}>Overload B (Hydrological Risk):</span>
                      <span className="mono" style={{ fontWeight: 700, color: 'var(--accent-rose)' }}>
                        {hazardResult.hazardHydrological} ({hazardResult.alertHydrological})
                      </span>
                    </div>
                  </div>
                )}
              </div>

              {/* Namespaces & Friend Class Inspector */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Eye size={20} /> Friend Class TelemetryAuditor & Call Semantics
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  Demonstrates private access override without public getters, protecting private calibration registers and raw ADC voltages.
                </p>

                <div style={{ display: 'flex', gap: '10px', marginBottom: '16px' }}>
                  <select 
                    value={auditStationId}
                    onChange={e => setAuditStationId(e.target.value)}
                    style={{
                      flex: 1,
                      padding: '8px 12px',
                      background: 'rgba(0,0,0,0.4)',
                      border: '1px solid var(--border-subtle)',
                      borderRadius: '6px',
                      color: '#fff'
                    }}
                  >
                    {gridData?.stations?.map(s => (
                      <option key={s.id} value={s.id}>{s.name} ({s.id})</option>
                    ))}
                  </select>
                  <button className="btn btn-secondary" onClick={() => runAudit(auditStationId)}>
                    Audit Target
                  </button>
                </div>

                {auditData && (
                  <pre className="mono" style={{
                    padding: '14px',
                    background: 'rgba(2, 6, 23, 0.9)',
                    borderRadius: '8px',
                    fontSize: '0.78rem',
                    color: '#38bdf8',
                    border: '1px solid var(--border-subtle)',
                    whiteSpace: 'pre-wrap',
                    lineHeight: 1.45
                  }}>
                    {auditData.auditReport}
                  </pre>
                )}
              </div>
            </div>
          </div>
        )}

        {/* ================================================================= */}
        {/* TAB 3: UNIT II - DYNAMIC MEMORY & OPERATOR OVERLOADING             */}
        {/* ================================================================= */}
        {activeTab === 'unit2' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
            <div>
              <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>Unit II: Encapsulation, Dynamic Memory & Operators</h2>
              <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                Strict data abstraction, dynamic sensor buffers (new[], deep copy, explicit destructors), static members, and overloaded binary +, &gt;, &lt;&lt;, &gt;&gt;.
              </p>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(450px, 1fr))', gap: '24px' }}>
              {/* Operator + Cluster Merging */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Layers size={20} /> Operator + : Regional Cluster Aggregation
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  Aggregates multiple isolated sensor station clusters into regional zone vectors using overloaded <code>operator+</code>.
                </p>

                <button className="btn btn-primary" onClick={runClusterMerge} style={{ width: '100%', marginBottom: '20px' }}>
                  Execute: ZoneA + ZoneB
                </button>

                {clusterMergeResult && (
                  <div style={{ background: 'rgba(0,0,0,0.3)', padding: '16px', borderRadius: '8px', border: '1px solid var(--border-subtle)' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px' }}>
                      <span style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Left Operand (Zone A):</span>
                      <span className="mono" style={{ color: 'var(--accent-cyan)' }}>{clusterMergeResult.zone1} (Hazard: {clusterMergeResult.hazard1})</span>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px' }}>
                      <span style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Right Operand (Zone B):</span>
                      <span className="mono" style={{ color: 'var(--accent-cyan)' }}>{clusterMergeResult.zone2} (Hazard: {clusterMergeResult.hazard2})</span>
                    </div>
                    <hr style={{ borderColor: 'var(--border-subtle)', margin: '12px 0' }} />
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '6px' }}>
                      <span style={{ fontWeight: 700 }}>Merged Zone Vector:</span>
                      <span className="mono" style={{ color: 'var(--accent-emerald)', fontWeight: 700 }}>{clusterMergeResult.mergedZone}</span>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '6px' }}>
                      <span style={{ color: 'var(--text-dim)' }}>Aggregated Total Nodes:</span>
                      <span className="mono">{clusterMergeResult.mergedTotalNodes} Sensor Nodes</span>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: 'var(--text-dim)' }}>Cumulative Hazard Sum:</span>
                      <span className="mono">{clusterMergeResult.mergedCumulativeHazard.toFixed(2)}</span>
                    </div>
                  </div>
                )}
              </div>

              {/* Dynamic TelemetryBuffer Visualizer */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <HardDrive size={20} /> Dynamic Memory Buffer (Rule of Five)
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  Heap allocations, parameterized constructor, deep copy constructor verification, and explicit destructors eliminating memory leaks.
                </p>

                <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
                  <div style={{ padding: '12px', background: 'rgba(255,255,255,0.02)', borderRadius: '8px', border: '1px solid var(--border-subtle)' }}>
                    <div style={{ fontSize: '0.8rem', color: 'var(--text-dim)', marginBottom: '4px' }}>1. Dynamic Constructor</div>
                    <code className="mono" style={{ fontSize: '0.75rem', color: '#a5b4fc' }}>
                      m_dataBuffer = new SensorSample[m_capacity]; // Heap allocation
                    </code>
                  </div>

                  <div style={{ padding: '12px', background: 'rgba(255,255,255,0.02)', borderRadius: '8px', border: '1px solid var(--border-subtle)' }}>
                    <div style={{ fontSize: '0.8rem', color: 'var(--text-dim)', marginBottom: '4px' }}>2. Deep Copy Constructor</div>
                    <code className="mono" style={{ fontSize: '0.75rem', color: '#a5b4fc' }}>
                      m_dataBuffer[i] = other.m_dataBuffer[i]; // Unique heap block
                    </code>
                  </div>

                  <div style={{ padding: '12px', background: 'rgba(255,255,255,0.02)', borderRadius: '8px', border: '1px solid var(--border-subtle)' }}>
                    <div style={{ fontSize: '0.8rem', color: 'var(--text-dim)', marginBottom: '4px' }}>3. Explicit Destructor</div>
                    <code className="mono" style={{ fontSize: '0.75rem', color: '#a5b4fc' }}>
                      delete[] m_dataBuffer; m_dataBuffer = nullptr;
                    </code>
                  </div>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* ================================================================= */}
        {/* TAB 4: UNIT III - INHERITANCE, CONTAINERSHIP & POLYMORPHISM       */}
        {/* ================================================================= */}
        {activeTab === 'unit3' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
            <div>
              <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>Unit III: Inheritance, Containership & Polymorphism</h2>
              <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                Abstract base classes, virtual base class resolution of the diamond problem, aggregation containership of PowerSubsystem, and runtime dynamic method dispatch (vtable/_vptr).
              </p>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(450px, 1fr))', gap: '24px' }}>
              {/* Diamond Problem Resolution Diagram */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Radio size={20} /> Virtual Base Class Diamond Problem Resolution
                </h3>

                <div style={{
                  display: 'flex',
                  flexDirection: 'column',
                  alignItems: 'center',
                  gap: '12px',
                  padding: '16px',
                  background: 'rgba(0,0,0,0.3)',
                  borderRadius: '8px',
                  fontFamily: 'var(--font-mono)'
                }}>
                  {/* Root */}
                  <div style={{
                    padding: '8px 16px',
                    background: '#1e293b',
                    border: '1px solid var(--accent-cyan)',
                    borderRadius: '6px',
                    fontSize: '0.8rem',
                    textAlign: 'center'
                  }}>
                    <strong>DisasterTelemetryDevice</strong>
                    <div style={{ fontSize: '0.7rem', color: 'var(--text-dim)' }}>[Virtual Root Base]</div>
                  </div>

                  <div style={{ display: 'flex', gap: '60px', color: 'var(--accent-blue)' }}>
                    <span>↙ virtual</span>
                    <span>↘ virtual</span>
                  </div>

                  {/* Intermediates */}
                  <div style={{ display: 'flex', gap: '20px' }}>
                    <div style={{
                      padding: '8px 14px',
                      background: '#1e293b',
                      border: '1px solid #3b82f6',
                      borderRadius: '6px',
                      fontSize: '0.75rem',
                      textAlign: 'center'
                    }}>
                      <strong>SolarPoweredNode</strong>
                      <div style={{ fontSize: '0.65rem', color: 'var(--text-dim)' }}>PV Solar Array</div>
                    </div>
                    <div style={{
                      padding: '8px 14px',
                      background: '#1e293b',
                      border: '1px solid #3b82f6',
                      borderRadius: '6px',
                      fontSize: '0.75rem',
                      textAlign: 'center'
                    }}>
                      <strong>WirelessLoRaTransceiver</strong>
                      <div style={{ fontSize: '0.65rem', color: 'var(--text-dim)' }}>868MHz Gateway</div>
                    </div>
                  </div>

                  <div style={{ display: 'flex', gap: '60px', color: 'var(--accent-blue)' }}>
                    <span>↘ public</span>
                    <span>↙ public</span>
                  </div>

                  {/* Derived Leaf */}
                  <div style={{
                    padding: '8px 20px',
                    background: 'rgba(16, 185, 129, 0.15)',
                    border: '2px solid var(--accent-emerald)',
                    borderRadius: '6px',
                    fontSize: '0.85rem',
                    textAlign: 'center',
                    color: '#34d399'
                  }}>
                    <strong>AutonomousHybridProbe</strong>
                    <div style={{ fontSize: '0.7rem', color: 'var(--text-dim)' }}>Single Root Instance Verified!</div>
                  </div>
                </div>
              </div>

              {/* Containership & Dynamic Dispatch */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Cpu size={20} /> Containership & Dynamic Dispatch (vtable)
                </h3>

                <div style={{ marginBottom: '16px', padding: '14px', background: 'rgba(0,0,0,0.3)', borderRadius: '8px' }}>
                  <div style={{ fontWeight: 700, fontSize: '0.85rem', marginBottom: '6px', color: 'var(--accent-amber)' }}>
                    Containership vs Classification
                  </div>
                  <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>
                    <code>BaseSensorNode</code> <strong>composes</strong> an independent <code>PowerSubsystem</code> object as a member rather than inheriting it, distinguishing composition (has-a) from classification (is-a).
                  </p>
                </div>

                <div style={{ padding: '14px', background: 'rgba(0,0,0,0.3)', borderRadius: '8px' }}>
                  <div style={{ fontWeight: 700, fontSize: '0.85rem', marginBottom: '6px', color: 'var(--accent-cyan)' }}>
                    Runtime Dynamic Method Dispatch
                  </div>
                  <code className="mono" style={{ fontSize: '0.75rem', color: '#a5b4fc', display: 'block', lineHeight: 1.5 }}>
                    std::vector&lt;BaseSensorNode*&gt; m_sensorNodes;<br />
                    for (auto* node : m_sensorNodes) &#123;<br />
                    &nbsp;&nbsp;node-&gt;readTelemetry(); // resolved via vtable/_vptr<br />
                    &nbsp;&nbsp;node-&gt;evaluateThreshold();<br />
                    &#125;
                  </code>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* ================================================================= */}
        {/* TAB 5: UNIT IV - PERSISTENCE, ROLLBACK & STL ROUTING              */}
        {/* ================================================================= */}
        {activeTab === 'unit4' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
            <div>
              <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>Unit IV: Streams, Persistence, Exception Safety & STL</h2>
              <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                Binary std::fstream persistence with seekp() transactional rollback, custom exception classes, STL priority-based evacuation routing with custom Functors and stream manipulators.
              </p>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(450px, 1fr))', gap: '24px' }}>
              {/* Binary Persistence & Transactional Rollback */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <Database size={20} /> Binary Incident Log & seekp() Rollback Test
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  Tests atomic serialization to <code>data/incidents.bin</code>. If a packet write fails or corrupts, <code>seekp()</code> rolls back the stream write pointer to the pre-transaction checkpoint.
                </p>

                <div style={{ display: 'flex', gap: '10px', marginBottom: '20px' }}>
                  <button className="btn btn-danger" onClick={runRollbackTest} style={{ flex: 1 }}>
                    <AlertTriangle size={16} /> Test seekp() Rollback Under Write Failure
                  </button>
                  <button className="btn btn-secondary" onClick={fetchIncidents}>
                    <RefreshCw size={16} /> Refresh Log
                  </button>
                </div>

                {rollbackResult && (
                  <div style={{
                    padding: '14px',
                    background: rollbackResult.rollbackSuccess ? 'rgba(16, 185, 129, 0.15)' : 'rgba(244, 63, 94, 0.15)',
                    border: `1px solid ${rollbackResult.rollbackSuccess ? '#10b981' : '#f43f5e'}`,
                    borderRadius: '8px',
                    marginBottom: '16px'
                  }}>
                    <div style={{ fontWeight: 700, fontSize: '0.85rem', marginBottom: '4px', color: rollbackResult.rollbackSuccess ? '#34d399' : '#f87171' }}>
                      {rollbackResult.rollbackSuccess ? '✓ TRANSACTION SAFELY ROLLED BACK' : '✗ ROLLBACK FAILED'}
                    </div>
                    <div style={{ fontSize: '0.8rem', color: 'var(--text-main)' }}>
                      {rollbackResult.explanation}
                    </div>
                    <div className="mono" style={{ fontSize: '0.75rem', color: 'var(--text-dim)', marginTop: '6px' }}>
                      Records Before: {rollbackResult.countBefore} | Records After: {rollbackResult.countAfter} (Unchanged)
                    </div>
                  </div>
                )}

                {/* Persistent Records Table */}
                <div style={{ maxHeight: '200px', overflowY: 'auto' }}>
                  <div style={{ fontSize: '0.8rem', fontWeight: 700, marginBottom: '8px', color: 'var(--text-muted)' }}>
                    Committed Incident Records ({incidents.length} on disk):
                  </div>
                  {incidents.length === 0 ? (
                    <div style={{ fontSize: '0.8rem', color: 'var(--text-dim)' }}>No records logged yet.</div>
                  ) : (
                    incidents.map(inc => (
                      <div key={inc.recordId} style={{
                        padding: '8px 12px',
                        background: 'rgba(0,0,0,0.3)',
                        borderRadius: '6px',
                        marginBottom: '6px',
                        fontSize: '0.75rem',
                        display: 'flex',
                        justifyContent: 'space-between',
                        alignItems: 'center'
                      }}>
                        <div>
                          <strong className="mono" style={{ color: 'var(--accent-cyan)' }}>#{inc.recordId} [{inc.stationId}]</strong>: {inc.summary}
                        </div>
                        <span className="mono" style={{ color: 'var(--accent-amber)' }}>CRC: {inc.checksum}</span>
                      </div>
                    ))
                  )}
                </div>
              </div>

              {/* Custom Exceptions & Fault Playground */}
              <div className="glass-panel" style={{ padding: '24px' }}>
                <h3 style={{ fontSize: '1.15rem', fontWeight: 700, marginBottom: '16px', display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                  <ShieldAlert size={20} /> Custom Exceptions Derived from std::exception
                </h3>
                <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '16px' }}>
                  Trigger and inspect C++ custom exception classes with full what() diagnostic message payloads.
                </p>

                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(3, 1fr)', gap: '8px', marginBottom: '20px' }}>
                  <button className="btn btn-secondary" style={{ fontSize: '0.75rem' }} onClick={() => runFaultInjection('sensor')}>
                    SensorFailureException
                  </button>
                  <button className="btn btn-secondary" style={{ fontSize: '0.75rem' }} onClick={() => runFaultInjection('checksum')}>
                    ChecksumMismatch
                  </button>
                  <button className="btn btn-secondary" style={{ fontSize: '0.75rem' }} onClick={() => runFaultInjection('overflow')}>
                    ThresholdOverflow
                  </button>
                </div>

                {faultResult && (
                  <div style={{
                    padding: '14px',
                    background: 'rgba(2, 6, 23, 0.9)',
                    borderRadius: '8px',
                    border: '1px solid var(--accent-rose)'
                  }}>
                    <div style={{ color: 'var(--accent-rose)', fontWeight: 700, fontSize: '0.85rem', marginBottom: '4px' }}>
                      Exception Caught: {faultResult.exceptionThrown} ({faultResult.code})
                    </div>
                    <pre className="mono" style={{ fontSize: '0.78rem', color: '#f87171', whiteSpace: 'pre-wrap' }}>
                      {faultResult.message}
                    </pre>
                  </div>
                )}
              </div>

              {/* Priority Evacuation Routing Matrix (STL std::sort with Functor) */}
              <div className="glass-panel" style={{ gridColumn: '1 / -1', padding: '24px' }}>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px' }}>
                  <div>
                    <h3 style={{ fontSize: '1.15rem', fontWeight: 700, display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--accent-cyan)' }}>
                      <Compass size={20} /> Evacuation Routing Matrix (STL std::sort &amp; EvacuationPriorityFunctor)
                    </h3>
                    <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>
                      Routes are sorted dynamically by highest risk score using a custom C++ Functor object. Formatted via custom stream manipulators.
                    </p>
                  </div>
                  <span className="badge badge-green">STL Containers: std::vector &amp; std::map</span>
                </div>

                <div style={{ overflowX: 'auto' }}>
                  <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '0.85rem', fontFamily: 'var(--font-mono)' }}>
                    <thead>
                      <tr style={{ background: 'rgba(0,0,0,0.4)', textAlign: 'left', color: 'var(--accent-cyan)' }}>
                        <th style={{ padding: '10px 14px' }}>Priority</th>
                        <th style={{ padding: '10px 14px' }}>Route ID</th>
                        <th style={{ padding: '10px 14px' }}>Source Sector</th>
                        <th style={{ padding: '10px 14px' }}>Safe Haven Dest</th>
                        <th style={{ padding: '10px 14px' }}>Dist (km)</th>
                        <th style={{ padding: '10px 14px' }}>Hazard</th>
                        <th style={{ padding: '10px 14px' }}>Transit Cap</th>
                        <th style={{ padding: '10px 14px' }}>Bridge Status</th>
                        <th style={{ padding: '10px 14px' }}>Risk Score</th>
                      </tr>
                    </thead>
                    <tbody>
                      {routes.map((r, idx) => (
                        <tr key={r.routeId} style={{
                          borderBottom: '1px solid var(--border-subtle)',
                          background: idx === 0 ? 'rgba(244, 63, 94, 0.1)' : 'transparent'
                        }}>
                          <td style={{ padding: '10px 14px', fontWeight: 700, color: idx === 0 ? '#f43f5e' : 'var(--text-dim)' }}>
                            #{idx + 1} {idx === 0 && '⚡ CRITICAL'}
                          </td>
                          <td style={{ padding: '10px 14px', color: 'var(--accent-blue)', fontWeight: 600 }}>{r.routeId}</td>
                          <td style={{ padding: '10px 14px' }}>{r.source}</td>
                          <td style={{ padding: '10px 14px' }}>{r.safeHaven}</td>
                          <td style={{ padding: '10px 14px' }}>{r.lengthKm} km</td>
                          <td style={{ padding: '10px 14px', color: r.hazard > 0.7 ? 'var(--accent-rose)' : 'var(--accent-amber)' }}>
                            {(r.hazard * 100).toFixed(0)}%
                          </td>
                          <td style={{ padding: '10px 14px' }}>{r.busCapacity} Buses</td>
                          <td style={{ padding: '10px 14px' }}>
                            {r.bridgeIntact ? (
                              <span style={{ color: '#10b981' }}>✓ Intact</span>
                            ) : (
                              <span style={{ color: '#f43f5e', fontWeight: 700 }}>✗ Damaged</span>
                            )}
                          </td>
                          <td style={{ padding: '10px 14px', fontWeight: 700, color: 'var(--accent-cyan)' }}>
                            {r.riskScore.toFixed(1)}
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* ================================================================= */}
        {/* TAB 6: C++ ACADEMIC VERIFICATION TERMINAL                         */}
        {/* ================================================================= */}
        {activeTab === 'terminal' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
              <div>
                <h2 style={{ fontSize: '1.5rem', fontWeight: 800 }}>C++ Academic Verification Console</h2>
                <p style={{ color: 'var(--text-muted)', fontSize: '0.875rem' }}>
                  Direct output from <code>alf_ewg_engine.exe --demo</code> compiled with GCC MinGW (C++14).
                </p>
              </div>
              <button className="btn btn-primary" onClick={fetchDemoOutput} disabled={loading}>
                <RefreshCw size={16} className={loading ? "spin" : ""} />
                Re-run Full C++ Verification Suite
              </button>
            </div>

            <div style={{
              background: '#020617',
              border: '1px solid var(--border-subtle)',
              borderRadius: '10px',
              padding: '20px',
              boxShadow: '0 10px 30px rgba(0,0,0,0.5)'
            }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '14px', paddingBottom: '12px', borderBottom: '1px solid rgba(255,255,255,0.06)' }}>
                <div style={{ width: '12px', height: '12px', borderRadius: '50%', background: '#ef4444' }}></div>
                <div style={{ width: '12px', height: '12px', borderRadius: '50%', background: '#f59e0b' }}></div>
                <div style={{ width: '12px', height: '12px', borderRadius: '50%', background: '#10b981' }}></div>
                <span className="mono" style={{ fontSize: '0.75rem', color: 'var(--text-dim)', marginLeft: '10px' }}>
                  powershell: bin\alf_ewg_engine.exe --demo
                </span>
              </div>
              <pre className="mono" style={{
                color: '#38bdf8',
                fontSize: '0.82rem',
                lineHeight: 1.5,
                whiteSpace: 'pre-wrap',
                maxHeight: '650px',
                overflowY: 'auto'
              }}>
                {demoOutput || 'Running C++ Academic Demo Suite...'}
              </pre>
            </div>
          </div>
        )}
      </main>

      {/* Footer */}
      <footer style={{
        background: 'var(--bg-secondary)',
        borderTop: '1px solid var(--border-subtle)',
        padding: '16px 28px',
        display: 'flex',
        justifyContent: 'space-between',
        alignItems: 'center',
        fontSize: '0.8rem',
        color: 'var(--text-dim)'
      }}>
        <div>
          MAIT CSE — Paper Code: <strong>CIC-211 (Object-Oriented Programming Using C++)</strong>
        </div>
        <div>
          Autonomous Landslide & Flash-Flood Early Warning Grid (Option 1)
        </div>
      </footer>
    </div>
  );
}
