
const state = {
  nodes: [
    {id:0,name:"Central Square",x:50,y:50},{id:1,name:"Tech Park",x:28,y:28},
    {id:2,name:"City Hospital",x:28,y:72},{id:3,name:"Railway Station",x:50,y:20},
    {id:4,name:"University Gate",x:50,y:80},{id:5,name:"Airport Junction",x:78,y:72},
    {id:6,name:"Market Street",x:72,y:35},{id:7,name:"River Bridge",x:82,y:18}
  ],
  roads: [
    [0,1,8],[0,2,12],[0,6,7],[1,3,10],[1,4,9],[2,4,6],
    [2,5,15],[3,5,11],[3,6,5],[4,6,8],[4,7,13],[5,7,9],[6,7,10]
  ],
  route: []
};

const $ = s => document.querySelector(s);
const $$ = s => [...document.querySelectorAll(s)];

function toast(msg){
  const t=$("#toast"); t.textContent=msg; t.classList.add("toast-show");
  setTimeout(()=>t.classList.remove("toast-show"),2200);
}
function adjacency(){
  const a=Array.from({length:state.nodes.length},()=>[]);
  state.roads.forEach(([u,v,w])=>{a[u].push([v,w]);a[v].push([u,w]);});
  return a;
}
function dijkstra(source,target){
  const a=adjacency(), d=Array(state.nodes.length).fill(Infinity), p=Array(state.nodes.length).fill(-1), used=Array(state.nodes.length).fill(false);
  d[source]=0;
  for(let k=0;k<state.nodes.length;k++){
    let u=-1;
    for(let i=0;i<d.length;i++) if(!used[i]&&(u<0||d[i]<d[u])) u=i;
    if(u<0||d[u]===Infinity) break;
    used[u]=true;
    for(const [v,w] of a[u]) if(d[u]+w<d[v]){d[v]=d[u]+w;p[v]=u;}
  }
  const path=[]; for(let v=target;v>=0;v=p[v]){path.push(v);if(v===source)break;}
  if(path[path.length-1]!==source) return null;
  return {cost:d[target],path:path.reverse()};
}
function bfs(source){
  const a=adjacency(), seen=Array(state.nodes.length).fill(false), q=[source], order=[];seen[source]=true;
  while(q.length){const u=q.shift();order.push(u);for(const [v] of a[u])if(!seen[v]){seen[v]=true;q.push(v);}}
  return order;
}
function dfs(source){
  const a=adjacency(), seen=Array(state.nodes.length).fill(false), order=[];
  function visit(u){seen[u]=true;order.push(u);for(const [v] of a[u])if(!seen[v])visit(v);}
  visit(source); return order;
}
function mst(){
  const edges=state.roads.map(([from,to,weight])=>({from,to,weight})).sort((a,b)=>a.weight-b.weight);
  const parent=state.nodes.map((_,i)=>i), rank=state.nodes.map(()=>0);
  function find(x){return parent[x]===x?x:(parent[x]=find(parent[x]));}
  function unite(a,b){a=find(a);b=find(b);if(a===b)return false;if(rank[a]<rank[b])[a,b]=[b,a];parent[b]=a;if(rank[a]===rank[b])rank[a]++;return true;}
  const out=[];let cost=0;for(const e of edges)if(unite(e.from,e.to)){out.push(e);cost+=e.weight;}
  return {edges:out,cost,connected:out.length===state.nodes.length-1};
}
function roadStatus(w){return w<=10?["LOW","traffic-low"]:w<=20?["MODERATE","traffic-mid"]:["CONGESTED","traffic-high"]}

function drawMap(containerId, highlight=[]){
  const el=$(containerId); if(!el)return; el.innerHTML="";
  const activeEdges=new Set();
  for(let i=0;i<highlight.length-1;i++) activeEdges.add(`${Math.min(highlight[i],highlight[i+1])}-${Math.max(highlight[i],highlight[i+1])}`);
  state.roads.forEach(([u,v,w])=>{
    const a=state.nodes[u],b=state.nodes[v], dx=b.x-a.x,dy=b.y-a.y;
    const road=document.createElement("div");road.className="road";
    if(activeEdges.has(`${Math.min(u,v)}-${Math.max(u,v)}`))road.classList.add("active");
    road.style.left=a.x+"%";road.style.top=a.y+"%";road.style.width=Math.hypot(dx,dy)+"%";road.style.transform=`rotate(${Math.atan2(dy,dx)*180/Math.PI}deg)`;
    el.appendChild(road);
  });
  state.nodes.forEach(n=>{
    const node=document.createElement("div");node.className="node"+(highlight.includes(n.id)?" active":"");
    node.style.left=n.x+"%";node.style.top=n.y+"%";node.title=`${n.id}: ${n.name}`;
    node.innerHTML=`<span class="node-label">${n.id} · ${n.name}</span>`;el.appendChild(node);
  });
}
function populateSelects(){
  ["#routeSource","#routeTarget","#emergencySource"].forEach(sel=>{
    const el=$(sel);if(!el)return;el.innerHTML=state.nodes.map(n=>`<option value="${n.id}">${n.id} — ${n.name}</option>`).join("");
  });
  $("#routeTarget").value="5";
}
function renderTraffic(){
  $("#trafficTable").innerHTML=state.roads.map((r,i)=>{
    const [u,v,w]=r,[status,cls]=roadStatus(w);
    return `<tr><td>${state.nodes[u].name} ↔ ${state.nodes[v].name}</td><td><input class="weight-input" id="w${i}" type="number" min="1" max="999" value="${w}"></td><td class="${cls}">${status}</td><td><button class="small-btn" onclick="updateWeight(${i})">UPDATE</button></td></tr>`;
  }).join("");
}
window.updateWeight=function(i){
  const val=Math.max(1,Math.min(999,Number($(`#w${i}`).value)||1));state.roads[i][2]=val;renderAll();toast("Traffic weight updated");
};
function renderStats(){
  $("#nodeCount").textContent=state.nodes.length;$("#roadCount").textContent=state.roads.length;
  $("#avgTraffic").textContent=Math.round(state.roads.reduce((s,r)=>s+r[2],0)/state.roads.length);
  const connected=bfs(0).length===state.nodes.length;$("#connectivity").textContent=connected?"100%":"DEGRADED";$("#health").textContent=connected?"100%":"75%";$("#healthBar").style.width=connected?"100%":"75%";
}
function renderAll(){renderStats();renderTraffic();populateSelects();drawMap("#map",state.route);drawMap("#routeMap",state.route);}
$$(".nav").forEach(btn=>btn.onclick=()=>{$$(".nav").forEach(x=>x.classList.remove("active"));btn.classList.add("active");$$(".view").forEach(x=>x.classList.remove("active-view"));$("#"+btn.dataset.view).classList.add("active-view");if(btn.dataset.view==="routes")drawMap("#routeMap",state.route);});
$("#findRoute").onclick=()=>{
  const s=+$("#routeSource").value,t=+$("#routeTarget").value,r=dijkstra(s,t);state.route=r?r.path:[];
  const box=$("#routeResult");box.classList.remove("hidden");
  if(!r){box.innerHTML="<b style='color:#ff4f81'>NO ROUTE</b>";}else{box.innerHTML=`<b style="color:#39ff88">FASTEST ROUTE FOUND</b><br>Traffic cost: <b>${r.cost}</b><br>${r.path.map(i=>state.nodes[i].name).join(" → ")}`;toast("Dijkstra route calculated");}
  drawMap("#routeMap",state.route);drawMap("#map",state.route);
};
$("#runBfs").onclick=()=>{$("#emergencyResult").innerHTML="<b style='color:#00eaff'>BFS ORDER</b><br>"+bfs(+$(`#emergencySource`).value).map(i=>state.nodes[i].name).join(" → ");toast("BFS clearance completed");};
$("#runDfs").onclick=()=>{$("#emergencyResult").innerHTML="<b style='color:#b86cff'>DFS ORDER</b><br>"+dfs(+$(`#emergencySource`).value).map(i=>state.nodes[i].name).join(" → ");toast("DFS validation completed");};
$("#runMst").onclick=()=>{
  const r=mst();$("#mstResult").innerHTML=`<div class="stat"><span>TOTAL INFRASTRUCTURE COST</span><b class="green">${r.cost}</b></div><div class="stat"><span>BACKBONE EDGES</span><b>${r.edges.length}</b></div>`+
  r.edges.map(e=>`<div class="mst-edge">${state.nodes[e.from].name} ↔ ${state.nodes[e.to].name} <b>cost ${e.weight}</b></div>`).join("");
  toast("Kruskal MST optimized");
};
setInterval(()=>$("#clock").textContent=new Date().toLocaleTimeString(),1000);
renderAll();
