async function loadNetworks() {
  const response = await fetch('/scan');
  const data = await response.json();

  const table = document.getElementById('wifiTable');
  table.innerHTML = '';

  document.getElementById('count').innerText = data.length;

  if(data.length > 0) {
    let strongest = data.reduce((a, b) => a.rssi > b.rssi ? a : b);
    document.getElementById('strongest').innerText = strongest.ssid || 'Hidden';
  }

  data.sort((a, b) => b.rssi - a.rssi);

  data.forEach(net => {
    const row = document.createElement('tr');

    let signalClass = 'signal-bad';

    if(net.rssi > -60) {
      signalClass = 'signal-good';
    } else if(net.rssi > -75) {
      signalClass = 'signal-medium';
    }

    row.innerHTML = `
      <td>${net.ssid || 'Hidden Network'}</td>
      <td class="${signalClass}">${net.rssi} dBm</td>
      <td>${net.channel}</td>
      <td>${net.encryption}</td>
    `;

    table.appendChild(row);
  });
}

loadNetworks();
setInterval(loadNetworks, 5000);