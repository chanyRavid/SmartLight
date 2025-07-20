


import { useState } from "react";
import React from 'react';

export default function Welcome() {
  const [brightness, setBrightness] = useState(128);
  const [turnOffHour, setTurnOffHour] = useState('');
  const [selectedImage, setSelectedImage] = useState(null);
  const [ocrResult, setOcrResult] = useState(null);
  const [selectedColor, setSelectedColor] = useState('#ffffff');

  const sendColorChange = async () => {
    try {
      const response = await fetch('http://192.168.1.101/color', {
        method: 'POST',
      });
      if (!response.ok) throw new Error('Failed to change color');
    } catch (error) {
      console.error('Error changing color:', error);
    }
  };

  const handleSliderChange = async (event) => {
    const newValue = event.target.value;
    setBrightness(newValue);

    try {
      const response = await fetch('http://192.168.1.101/brightness', {
        method: 'POST',
        headers: {
          'Content-Type': 'text/plain',
        },
        body: newValue.toString(),
      });

      if (!response.ok) {
        throw new Error('Failed to update brightness');
      }
    } catch (error) {
      console.error('Error updating brightness:', error);
    }
  };

  const toggleLight = async () => {
    try {
      const response = await fetch('http://192.168.1.101/toggle', {
        method: 'POST',
      });
      if (!response.ok) throw new Error('Failed to toggle light');
      console.log('Light toggle command sent');
    } catch (error) {
      console.error('Error toggling light:', error);
    }
  };
  const sendTurnOffHour = async () => {
    if (!turnOffHour) {
      alert('אנא בחר שעה');
      return;
    }
    
    try {
      const response = await fetch('http://192.168.1.101/turnoff', {
        method: 'POST',
        headers: {
          'Content-Type': 'text/plain',
        },
        body: turnOffHour,
      });
  
      if (!response.ok) {
        throw new Error('Failed to update turn off time');
      }
      
      alert('כיבוי עודכן בהצלחה לשעה ' + turnOffHour);
    } catch (error) {
      console.error('Error updating turn off time:', error);
      alert('שגיאה בעדכון שעת הכיבוי');
    }
  };
  
  const handleImageSelect = (event) => {
    const file = event.target.files[0];
    if (file) {
      setSelectedImage(file);
      setOcrResult(null); 
    }
  };
  
  const uploadImage = async () => {
    if (!selectedImage) {
      alert('אנא בחר תמונה תחילה');
      return;
    }
  
    try {
      const formData = new FormData();
      formData.append('image', selectedImage);
  
      const response = await fetch('http://localhost:5000/upload-image', {
        method: 'POST',
        body: formData
      });
  
      if (!response.ok) {
        throw new Error('Failed to upload image');
      }
  
      const result = await response.json();
      setOcrResult(result);
      
      if (result.esp32_success) {
        alert('פקודה בוצעה בהצלחה!');
      } else {
        alert('הפקודה נקראה אבל לא בוצעה ב-ESP32');
      }
      
    } catch (error) {
      console.error('Error uploading image:', error);
      alert('שגיאה בהעלאת התמונה');
    }
  };

  // פונקציה להמרת צבע HEX ל-RGB
const hexToRgb = (hex) => {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
};

// פונקציה לשליחת צבע RGB ל-ESP32
const sendCustomColor = async (hexColor) => {
  const rgb = hexToRgb(hexColor);
  if (!rgb) {
    alert('צבע לא תקין');
    return;
  }

  try {
    const response = await fetch('http://192.168.1.101/setrgb', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: `r=${rgb.r}&g=${rgb.g}&b=${rgb.b}`
    });

    if (!response.ok) {
      throw new Error('Failed to set custom color');
    }
    
    console.log(`Color set: R=${rgb.r}, G=${rgb.g}, B=${rgb.b}`);
  } catch (error) {
    console.error('Error setting custom color:', error);
    alert('שגיאה בהגדרת הצבע');
  }
};

// טיפול בשינוי צבע
const handleColorChange = (event) => {
  const color = event.target.value;
  setSelectedColor(color);
  sendCustomColor(color);
};

  return (
    <div style={styles.container}>
      <div style={styles.content}>
        <h1 style={styles.title}>Welcome to the SmartLight</h1>
        <h3 style={styles.subtitle}>Here you can control the lighting yourself.</h3>
        <div style={styles.buttonContainer}>
          <button style={styles.button} onClick={sendColorChange}>
            <span style={styles.buttonIcon}>🎨</span>
            Change color
          </button>
          {/* בוחר צבעים חדש - פשוט ונקי */}
<div style={{
  padding: '20px',
  background: 'rgba(255, 255, 255, 0.08)',
  borderRadius: '15px',
  border: '1px solid rgba(255, 255, 255, 0.2)',
  marginBottom: '15px'
}}>
  <label style={{
    color: 'white',
    fontSize: '18px',
    fontWeight: '500',
    display: 'block',
    marginBottom: '15px',
  }}>
    🌈 Select color:
  </label>
  <div style={{
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    gap: '15px',
  }}>
    <input
      type="color"
      value={selectedColor}
      onChange={handleColorChange}
      style={{
        width: '80px',
        height: '50px',
        border: '3px solid rgba(255, 255, 255, 0.3)',
        borderRadius: '12px',
        cursor: 'pointer',
        transition: 'all 0.3s ease',
        background: 'transparent',
      }}
    />
    <span style={{
      color: 'white',
      fontSize: '16px',
      fontFamily: 'monospace',
      background: 'rgba(0, 0, 0, 0.3)',
      padding: '10px 15px',
      borderRadius: '8px',
      border: '1px solid rgba(255, 255, 255, 0.2)',
    }}>
      {selectedColor}
    </span>
  </div>
</div>

          <label htmlFor="brightnessRange" style={{ color: 'white' }}>
            Clarity: {brightness}
          </label>.
          <input
            id="brightnessRange"
            type="range"
            min="0"
            max="255"
            value={brightness}
            onChange={handleSliderChange}
          />


          <button style={styles.button} onClick={toggleLight}>
            <span style={styles.buttonIcon}>⚡</span>
            Switch on/off
          </button>

          <label htmlFor="turnOffHourInput" style={{ color: 'white' }}>
            Select a shutdown time:
          </label>
          <input
            id="turnOffHourInput"
            type="time"
            value={turnOffHour}
            onChange={(e) => setTurnOffHour(e.target.value)}
            style={{ padding: '8px', borderRadius: '8px', border: '1px solid #ccc' }}
          />
          <button style={styles.button} onClick={sendTurnOffHour}>
            <span style={styles.buttonIcon}>🕙</span>
            Set shutdown time
          </button>
          
<div style={{ marginTop: '20px', padding: '20px', background: 'rgba(255, 255, 255, 0.1)', borderRadius: '12px' }}>
  <h4 style={{ color: 'white', marginBottom: '15px' }}>You can upload a picture and the system will turn on the light according to your requirement. </h4>
  
  <div style={{ marginBottom: '15px' }}>
  <input
    type="file"
    accept="image/*"
    onChange={handleImageSelect}
    id="fileInput"
    style={{ display: 'none' }}
  />
  <label 
    htmlFor="fileInput" 
    style={{
      ...styles.button,
      cursor: 'pointer',
      background: 'rgba(255, 255, 255, 0.1)',
      border: '2px dashed rgba(255, 255, 255, 0.3)',
      padding: '20px',
      textAlign: 'center',
      transition: 'all 0.3s ease'
    }}
  >
    <span style={styles.buttonIcon}>📷</span>
    {selectedImage ? selectedImage.name : 'Select a picture'}
  </label>
</div>
  
  <button 
    style={styles.button} 
    onClick={uploadImage}
    disabled={!selectedImage}
  >
    <span style={styles.buttonIcon}>📤</span>
    Send a picture
  </button>
  
  {ocrResult && (
    <div style={{ marginTop: '15px', padding: '10px', background: 'rgba(255, 255, 255, 0.1)', borderRadius: '8px' }}>
      <h5 style={{ color: 'white', marginBottom: '10px' }}>תוצאה:</h5>
      <pre style={{ color: 'white', fontSize: '12px', textAlign: 'right' }}>
        {JSON.stringify(ocrResult, null, 2)}
      </pre>
    </div>
  )}
</div>
        </div>
      </div>
    </div>
  );
}

const styles = {
  container: {
    minHeight: '100vh',
    background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    padding: '20px',
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif',
  },
  content: {
    textAlign: 'center',
    background: 'rgba(255, 255, 255, 0.1)',
    backdropFilter: 'blur(10px)',
    borderRadius: '20px',
    padding: '40px',
    border: '1px solid rgba(255, 255, 255, 0.2)',
    boxShadow: '0 8px 32px rgba(0, 0, 0, 0.3)',
    maxWidth: '500px',
    width: '100%',
  },
  title: {
    fontSize: '36px',
    color: 'white',
    margin: '0 0 20px 0',
    fontWeight: 'bold',
    textShadow: '2px 2px 4px rgba(0,0,0,0.3)',
  },
  subtitle: {
    fontSize: '18px',
    color: 'rgba(255, 255, 255, 0.9)',
    margin: '0 0 40px 0',
    fontWeight: '400',
  },
  buttonContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '15px',
  },
  button: {
    background: 'rgba(255, 255, 255, 0.15)',
    border: '1px solid rgba(255, 255, 255, 0.3)',
    borderRadius: '12px',
    padding: '16px 24px',
    color: 'white',
    fontSize: '16px',
    fontWeight: '500',
    cursor: 'pointer',
    transition: 'all 0.3s ease',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    gap: '10px',
    backdropFilter: 'blur(5px)',
  },
  buttonIcon: {
    fontSize: '20px',
  },
};

const styleSheet = document.createElement("style");
styleSheet.innerText = `
  button:hover {
    background: rgba(255, 255, 255, 0.25) !important;
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.2);
  }

  button:active {
    transform: translateY(0px);
  }
`;
document.head.appendChild(styleSheet);
